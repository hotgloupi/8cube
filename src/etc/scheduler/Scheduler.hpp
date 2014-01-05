#ifndef  ETC_SCHEDULER_SCHEDULER_HPP
# define ETC_SCHEDULER_SCHEDULER_HPP

# include <etc/memory.hpp>

# include <functional>
# include <string>

namespace etc { namespace scheduler {

	struct Context;
	class Scheduler
	{
	public:
		typedef std::function<void(Context&)> handler_type;
	private:
		struct Impl;
		std::unique_ptr<Impl> _this;

	public:
		Scheduler();
		~Scheduler();

		void run();
		void spawn(std::string name, handler_type const& fn);
		void sleep(int sec);
	};


}}

#endif
