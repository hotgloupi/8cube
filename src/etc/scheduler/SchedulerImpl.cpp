#include "SchedulerImpl.hpp"

#include <etc/log.hpp>

#include <unordered_map>

namespace etc { namespace scheduler {

	ETC_LOG_COMPONENT("etc.scheduler.Scheduler");

	typedef std::unordered_map<std::thread::id, Scheduler*> schedulers_map;

	namespace {

		inline schedulers_map& schedulers()
		{
			static schedulers_map scheds;
			return scheds;
		}

	}

	void Scheduler::Impl::current_scheduler(Scheduler* sched)
	{
		ETC_LOG.debug("Setting the current scheduler to", sched);
		schedulers()[std::this_thread::get_id()] = sched;
	}

	Scheduler& Scheduler::Impl::current_scheduler()
	{
		auto it = schedulers().find(std::this_thread::get_id());
		if (it != schedulers().end() && it->second != nullptr)
			return *it->second;
		throw exception::Exception("No scheduler available");
	}

}}
