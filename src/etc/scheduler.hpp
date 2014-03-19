#ifndef  ETC_SCHEDULER_HPP
# define ETC_SCHEDULER_HPP

# include <etc/scheduler/Scheduler.hpp>

namespace etc { namespace scheduler {

	Scheduler& current();

	template<typename Callable>
	auto spawn(std::string name, Callable&& cb)
		-> decltype(std::forward<Callable>(cb)())
	{ current().spawn(std::move(name), [&] (Context&) { std::forward<Callable>(cb)(); }); }

	template<typename Callable>
	auto spawn(std::string name, Callable&& cb)
		-> decltype(std::forward<Callable>(cb)(std::declval<Context&>()))
	{ current().spawn(std::move(name), cb); }

}}

#endif
