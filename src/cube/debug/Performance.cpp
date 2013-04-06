#include "Performance.hpp"

#include <etc/print.hpp>

#include <boost/thread/tss.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/mutex.hpp>

#include <cassert>
#include <chrono>
#include <cstring>
#include <list>
#include <iomanip>
#include <stack>
#include <unordered_map>

namespace std {

	template<>
	struct hash<cube::debug::Performance::Info>
	{
		size_t operator ()(cube::debug::Performance::Info const& info) const
		{
			static std::hash<std::string> h{};
			return h(std::string{info.file}) + info.line;
		}
	};


	bool operator ==(cube::debug::Performance::Info const& lhs,
	                 cube::debug::Performance::Info const& rhs)
	{
		return (
			lhs.line == rhs.line and
			strcmp(lhs.function, rhs.function) == 0 and
			strcmp(lhs.file, rhs.file) == 0 and
			strcmp(lhs.name, rhs.name) == 0
		);
	}

}

namespace cube { namespace debug {

	typedef std::chrono::system_clock clock_type;

	namespace {

		struct CallStat
		{
			uint64_t                count;
			clock_type::duration    total_time;

			CallStat()
				: count{0}
				, total_time{0}
			{}
		};

		struct Timer
		{
			CallStat*               stat_ptr;
			clock_type::time_point  start;

			Timer(CallStat* ptr)
				: stat_ptr{ptr}
				, start{clock_type::now()}
			{}
		};

	} //!anonymous

	struct Performance::Impl
		: public boost::basic_lockable_adapter<boost::mutex>
	{
		typedef std::unordered_map<Performance::Info, CallStat>     StatMap;
		typedef std::unordered_map<Performance::id_type, Timer>     TimerMap;
		typedef std::stack<Info*>                                   CallStack;
		typedef std::unordered_set<Info const*>                     RootSet;

		Performance::id_type                    next_id;
		StatMap                                 stats;
		TimerMap                                timers;
		boost::thread_specific_ptr<CallStack>   stack;
		RootSet                                 roots;
	};

	Performance&
	Performance::instance()
	{
		static Performance manager;
		return manager;
	}

	Performance::Performance()
		: _this{new Impl}
	{
		_this->next_id = 1;
	}

	Performance::~Performance()
	{}

	Performance::id_type
	Performance::begin(Info&& info)
	{
		if (_this->stack.get() == nullptr)
			_this->stack.reset(new Impl::CallStack{});
		Impl::CallStack& stack = *_this->stack.get();

		CallStat* stat_ptr = nullptr;
		Info const* info_ptr = nullptr;
		id_type id;

		{ // locked section

			boost::lock_guard<Impl> guard(*_this);
			id = _this->next_id++;

			// Insert a new CallStat if needed, but retreive info and stat ptr.
			{
				auto res = _this->stats.insert({std::move(info), CallStat{}});
				assert(res.second == false or res.first->first._children.size() == 0);
				info_ptr = &res.first->first;
				stat_ptr = &res.first->second;
			}

			assert(info_ptr != nullptr);
			assert(stat_ptr != nullptr);

			// Insert a new timer for the stat found earlier.
			{
				auto res = _this->timers.insert({id, Timer{stat_ptr}});
				assert(res.second == true && "timers insert failed");
			}

			// If the stack is not empty, add info as a child of the last call.
			if (!stack.empty())
				stack.top()->add_child(info_ptr);
			else
				_this->roots.insert(info_ptr);

		} // end of locked section

		// We do not alter the way info instance is hashed, but we'll need to
		// add children later if the scope has a subsection.
		stack.push(const_cast<Info*>(info_ptr));

		return id;
	}

	void
	Performance::end(id_type const id)
	{
		{ // locked section
			boost::lock_guard<Impl> guard(*_this);
			// Retreive the timer.
			auto it = _this->timers.find(id);
			assert(it != _this->timers.end());
			auto& timer = it->second;

			// Update stat
			assert(timer.stat_ptr != nullptr);
			timer.stat_ptr->total_time += clock_type::now() - timer.start;
			timer.stat_ptr->count++;

			// Remove the timer from the map.
			_this->timers.erase(it);
		}

		assert(_this->stack.get() != nullptr);
		// Pop the last stack element.
		assert(not _this->stack->empty());
		_this->stack->pop();
	}

	void
	Performance::dump()
	{
		etc::print("Performances stats:");
		unsigned int max_name_len = 0;
		for (auto const& pair: _this->stats)
		{
			auto len = strlen(pair.first.name);
			if (len > max_name_len)
				max_name_len = len;
		}

		this->dump_set(_this->roots, max_name_len, 0);
	}

	void
	Performance::dump_set(std::unordered_set<Info const*> const& set,
	                      unsigned int const max_name_len,
	                      unsigned int const indent)
	{
		std::unordered_map<std::string, std::vector<Info const*>> by_name;
		for (Info const* info: set)
			by_name[info->name].push_back(info);
		std::vector<std::string> names;
		std::transform(
			by_name.begin(), by_name.end(),
			std::back_inserter(names),
			[&] (decltype(by_name)::value_type const& pair) {return pair.first;}
		);

		auto duration_sum = [&] (std::vector<Info const*> const& first_list) -> clock_type::duration {
			return std::accumulate(
				first_list.begin(), first_list.end(),
				clock_type::duration{},
				[&] (clock_type::duration const& prev, Info const* info) -> clock_type::duration {
					return prev + _this->stats[*info].total_time;
				}
			);
		};
		auto duration_greater = [&] (std::vector<Info const*> const& first_list,
		                             std::vector<Info const*> const& second_list) -> bool {
			return duration_sum(first_list) > duration_sum(second_list);
		};

		std::sort(
			names.begin(), names.end(),
			[&] (std::string const& first, std::string const& second) -> bool {
				return duration_greater(by_name[first], by_name[second]);
			}
		);
		for (auto const& name: names)
		{
			std::cout << std::setiosflags(std::ios::left) << std::setw(indent) << ' ' << name << std::endl;
			auto& list = by_name[name];
			std::sort(
				list.begin(), list.end(),
				[&] (Info const* first, Info const* second) -> bool {
					return _this->stats[*first].total_time > _this->stats[*second].total_time;
				}
			);
			for (Info const* info: list)
				this->dump_info(*info, max_name_len, indent + 2);
		}
	}


	void
	Performance::dump_info(Performance::Info const& info,
	                       unsigned int const max_name_len,
	                       unsigned int const indent)
	{

		auto& stat = _this->stats[info];
		std::cout << std::setiosflags(std::ios::left) << std::setfill(' ') << std::setw(indent + 2) << ' ' << info.function << std::endl;
		std::cout << std::setw(indent+3) << ' ' << " -> ";
		std::cout //<< std::setiosflags(std::ios::left) <<std::setw(10)
		          << stat.count
		          << " times, total: "
		          //<< std::setw(10)
		          << std::chrono::duration_cast<std::chrono::milliseconds>(stat.total_time).count()
		          << " ms, avg: "
		          << std::chrono::duration_cast<std::chrono::nanoseconds>(stat.total_time).count() / (stat.count ? stat.count : 1)
		          << " ns"
		          << std::endl;
		;
		this->dump_set(info._children, max_name_len, indent + 2);
	}
}}
