#include "Performance.hpp"

#include <etc/print.hpp>
#include <etc/log.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/tss.hpp>

#include <atomic>
#include <cassert>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <list>
#include <numeric>
#include <stack>
#include <thread>
#include <unordered_map>

namespace std {

	template<>
	struct hash<cube::debug::Performance::Info>
	{
		size_t operator ()(cube::debug::Performance::Info const& info) const
		{
			static std::hash<std::string> h{};
			return h(info.file) + info.line + (size_t)info.parent();
		}
	};

} //!std

namespace cube { namespace debug {

	ETC_LOG_COMPONENT("cube.debug.Performance");

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

			Timer(CallStat* ptr, clock_type::time_point now)
				: stat_ptr{ptr}
				, start{now}
			{}
		};

		std::string
		human_duration(clock_type::duration const& d)
		{
			using namespace std::chrono;
			if (duration_cast<seconds>(d).count() > 1)
				return std::to_string(
					duration_cast<milliseconds>(d).count() / 1000.0f
				) + " seconds";
			else if (duration_cast<milliseconds>(d).count() > 1)
				return std::to_string(
					duration_cast<microseconds>(d).count() / 1000.0f
				) + " milliseconds";
			return std::to_string(
				duration_cast<nanoseconds>(d).count()
			) + " nanoseconds";
		}

	} //!anonymous

	struct Performance::Impl
	{
		typedef std::unordered_map<Performance::Info, CallStat> StatMap;
		typedef std::unordered_map<Performance::id_type, Timer> TimerMap;
		typedef std::stack<Info*>                               CallStack;
		typedef std::unordered_map<std::thread::id, CallStack>  ThreadStackMap;
		typedef std::unordered_set<Info const*>                 RootSet;

		std::atomic<Performance::id_type>              next_id;
		StatMap                                        stats;
		TimerMap                                       timers;
		ThreadStackMap                                 stacks;
		RootSet                                        roots;

		boost::asio::io_service                        service;
		std::unique_ptr<boost::asio::io_service::work> work;
		std::thread                                    worker_thread;

		Impl()
			: next_id{1}
			, stats{}
			, timers{}
			, stacks{}
			, roots{}
			, service{}
			, work{new boost::asio::io_service::work{service}}
			, worker_thread{
				[this] { this->service.run(); }
			}
		{}
	};

	Performance&
	Performance::instance()
	{
		static Performance manager;
		return manager;
	}

	Performance::Performance()
		: _this{new Impl}
	{}

	Performance::~Performance()
	{
		this->shutdown();
	}

	Performance::id_type
	Performance::begin(Info&& info_) ETC_NOEXCEPT
	{
		try {
			auto const thread_id = std::this_thread::get_id();
			id_type const id = _this->next_id++;
			auto start_time = clock_type::now();
			_this->service.post([=] {
				Info info{std::move(info_)};
				Impl::CallStack& stack = _this->stacks[thread_id];

				CallStat* stat_ptr = nullptr;
				Info const* info_ptr = nullptr;

				// Set the info parent
				if (!stack.empty())
					info.set_parent(stack.top());

				// Insert a new CallStat if needed, but retreive info and stat ptr.
				{
					auto res = _this->stats.insert({std::move(info), {}});
					info_ptr = &res.first->first;
					stat_ptr = &res.first->second;
				}

				assert(info_ptr != nullptr);
				assert(stat_ptr != nullptr);

				// Insert a new timer for the stat found earlier.
				{
					auto res = _this->timers.insert({id, Timer{stat_ptr, start_time}});
					assert(res.second == true && "timers insert failed");
				}

				// If the stack is not empty, add info as a child of the last call.
				if (!stack.empty())
					stack.top()->add_child(info_ptr);
				else
					_this->roots.insert(info_ptr);

				// We do not alter the way info instance is hashed, but we'll need to
				// add children later if the scope has a subsection.
				stack.push(const_cast<Info*>(info_ptr));
			});
			return id;
		} catch (...) {
			ETC_LOG.warn("Couldn't begin a Performance section");
		}
		return 0;
	}

	void
	Performance::end(id_type const id) ETC_NOEXCEPT
	{
		try {
			auto const thread_id = std::this_thread::get_id();
			auto end_time = clock_type::now();

			_this->service.post([id, thread_id, end_time, this] {
				// Retreive the timer.
				auto it = _this->timers.find(id);
				assert(it != _this->timers.end());
				auto& timer = it->second;

				// Update stat
				assert(timer.stat_ptr != nullptr);
				timer.stat_ptr->total_time += end_time - timer.start;
				timer.stat_ptr->count++;

				// Remove the timer from the map.
				_this->timers.erase(it);

				// Pop the last stack element.
				assert(not _this->stacks[thread_id].empty());
				_this->stacks[thread_id].pop();
			});
		} catch (...) {
			ETC_LOG.warn("Couldn't end a Performance section");
		}
	}

	void
	Performance::shutdown()
	{
		if (_this->work != nullptr)
		{
			_this->work.reset();
			_this->worker_thread.join();
		}
	}

	void
	Performance::dump()
	{
		etc::print("Performances stats:");
		unsigned int max_name_len = 0;
		for (auto const& pair: _this->stats)
		{
			if (pair.first.name.size() > max_name_len)
				max_name_len = pair.first.name.size();
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
			auto& list = by_name[name];
			std::cout << std::setiosflags(std::ios::left) << std::setw(indent) << ' '
			          //<< std::setw(max_name_len + 1)
			          << "    " << "---[ " << name << " ] "
			          << human_duration(duration_sum(list))
			          << std::endl;
			std::sort(
				list.begin(), list.end(),
				[&] (Info const* first, Info const* second) -> bool {
					return _this->stats[*first].total_time > _this->stats[*second].total_time;
				}
			);
			for (Info const* info: list)
				this->dump_info(*info, max_name_len, indent + 4);
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
		          << human_duration(stat.total_time)
		          << ", avg: "
		          << human_duration(stat.total_time / (stat.count ? stat.count : 1))
		          << std::endl;
		;
		this->dump_set(info._children, max_name_len, indent + 2);
	}

}}
