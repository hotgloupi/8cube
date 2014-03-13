#include "scheduler.hpp"
#include "scheduler/SchedulerImpl.hpp"

#include <etc/exception.hpp>

namespace etc { namespace scheduler {

	Scheduler& current()
	{ return Scheduler::Impl::current_scheduler(); }

}}
