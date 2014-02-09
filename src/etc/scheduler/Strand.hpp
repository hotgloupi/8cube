#ifndef  ETC_SCHEDULER_STRAND_HPP
# define ETC_SCHEDULER_STRAND_HPP

# include "fwd.hpp"

# include <memory>
# include <functional>

namespace etc { namespace scheduler {

	class Strand
	{
	public:
		typedef std::function<void(Context&)> handler_type;
	private:
		Scheduler&             _scheduler;
		struct Impl;
		std::unique_ptr<Impl>  _this;

	private:
		friend class Scheduler;
		Strand(Scheduler& sched, std::unique_ptr<Impl> impl);
	public:
		Strand(Strand&& other);
		~Strand();

	public:
		void spawn(std::string name, handler_type fn);
	};

}}

#endif
