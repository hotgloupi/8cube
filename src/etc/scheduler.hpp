#ifndef  ETC_SCHEDULER_HPP
# define ETC_SCHEDULER_HPP

# include <etc/scheduler/Scheduler.hpp>

namespace etc { namespace scheduler {

	Scheduler& current();


	//template<typename Callable>
	//auto spawn(std::string name, Callable cb);

	// spawn implem
	namespace details {
		template<typename Callable>
		auto spawn_imp(std::string name, Callable cb, int)
			-> decltype(std::forward<Callable>(cb)())
		{
			return current().spawn(
				std::move(name),
				[&] (Context&) { std::forward<Callable>(cb)(); }
			);
		}

		template<typename Callable>
		auto spawn_imp(std::string name, Callable cb, long)
			-> decltype(std::forward<Callable>(cb)(std::declval<Context&>()))
		{ return current().spawn(std::move(name), cb); }
	}

	template<typename Callable>
	auto spawn(std::string name, Callable cb)
		-> decltype(details::spawn_imp(std::move(name), std::move(cb), 0))
	{ return details::spawn_imp(std::move(name), std::move(cb), 0); }

}}

#endif
