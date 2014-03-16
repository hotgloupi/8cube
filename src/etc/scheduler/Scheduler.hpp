#ifndef  ETC_SCHEDULER_SCHEDULER_HPP
# define ETC_SCHEDULER_SCHEDULER_HPP

# include "fwd.hpp"

# include <etc/memory.hpp>

# include <functional>
# include <string>

namespace etc { namespace scheduler {

	class Scheduler
	{
	public:
		typedef std::function<void(Context&)> handler_type;
		struct Impl;
	private:
		std::unique_ptr<Impl> _this;

	public:
		Scheduler();
		~Scheduler();

		void run();
		void stop();
		void spawn(std::string name, handler_type const& fn);
		void sleep(int sec);

		Strand strand();

	public:
		inline Impl& impl() { return *_this; }
		Context* current();
		Context& context();
	};


}}

#endif
