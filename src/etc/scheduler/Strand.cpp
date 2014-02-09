#include "Strand.hpp"
#include "StrandImpl.hpp"
#include "Scheduler.hpp"
#include "Context.hpp"

namespace etc { namespace scheduler {

	Strand::Strand(Scheduler& sched, std::unique_ptr<Impl> impl)
		: _scheduler(sched)
		, _this{std::move(impl)}
	{}

	Strand::Strand(Strand&& other)
		: _scheduler(other._scheduler)
		, _this{std::move(other._this)}
	{}

	Strand::~Strand()
	{}

	void Strand::spawn(std::string name, handler_type fn)
	{
		//_scheduler.spawn(
		//	std::move(name),
		//	_this->strand.wrap(fn)
		//);
	}

}}
