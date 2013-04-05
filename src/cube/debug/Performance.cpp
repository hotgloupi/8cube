#include "Performance.hpp"

#include <cassert>
#include <chrono>
#include <stack>
#include <unordered_map>

namespace std {

	template<>
	struct hash<cube::debug::Performance::Info>
	{
		size_t operator ()(cube::debug::Performance::Info const& info) const
		{
			static std::hash<std::string> h{};
			return h(info.file) + info.line;
		}
	};


	bool operator ==(cube::debug::Performance::Info const& lhs,
	                 cube::debug::Performance::Info const& rhs)
	{
		return (
			lhs.line == lhs.line and
			lhs.function == lhs.function and
			lhs.file == lhs.file and
			lhs.name == rhs.name
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
	{
		typedef std::unordered_map<Performance::Info, CallStat>     StatMap;
		typedef std::unordered_map<Performance::id_type, Timer>     TimerMap;
		typedef std::stack<Info*>                                   CallStack;

		Performance::id_type    next_id;
		StatMap                 stats;
		TimerMap                timers;
		CallStack               stack;
	};

	Performance&
	Performance::instance()
	{
		static Performance manager{};
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
		id_type const id = _this->next_id++;

		// Insert a new CallStat if needed, but retreive info and stat ptr.
		auto it = _this->stats.find(info);
		CallStat* stat_ptr = nullptr;
		Info const* info_ptr = nullptr;
		if (it == _this->stats.end())
		{
			auto res = _this->stats.insert({std::move(info), CallStat{}});
			info_ptr = &res.first->first;
			stat_ptr = &res.first->second;
		}
		else
		{
			info_ptr = &it->first;
			stat_ptr = &it->second;
		}
		assert(info_ptr != nullptr);
		assert(stat_ptr != nullptr);

		// Insert a new timer for the stat found earlier.
		auto res = _this->timers.insert({id, Timer{stat_ptr}});
		assert(res.second == true && "timers insert failed");

		// If the stack is not empty, add info as a child of the last call.
		if (_this->stack.empty() == false)
			_this->stack.top()->add_child(info_ptr);

		// We do not alter the way info instance is hashed, but we'll need to
		// reference children.
		_this->stack.push(const_cast<Info*>(info_ptr));

		return id;
	}

	void
	Performance::end(id_type const id)
	{
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

		// Pop the last stack element.
		assert(not _this->stack.empty());
		assert(&_this->stats[*_this->stack.top()] == timer.stat_ptr);
		_this->stack.pop();
	}

}}
