#ifndef  ETC_SCHEDULER_CONTEXT_HPP
# define ETC_SCHEDULER_CONTEXT_HPP

# include "fwd.hpp"

# include <etc/exception.hpp>
# include <etc/log.hpp>
# include <etc/scope_exit.hpp>

# include <boost/noncopyable.hpp>
# include <boost/coroutine/coroutine.hpp>

namespace etc { namespace scheduler {

	typedef boost::coroutines::coroutine<void()> coroutine_type;

	struct Context
		: private boost::noncopyable
	{
		ETC_LOG_COMPONENT("etc.scheduler.Context");

		coroutine_type* _yield;
		Context* _parent;
		std::string name;
		coroutine_type coro;
		std::exception_ptr exception;

		template<typename Handler>
		Context(std::string name,
		        Context* parent,
		        Handler&& hdlr)
			: _yield{nullptr}
			, _parent{parent}
			, name(std::move(name))
			, coro(
				[hdlr, this] (coroutine_type& ca) {
					ETC_LOG.debug("Initiated context", this->name, &ca, &this->coro);
					ca();
					ETC_LOG.debug("Awaken context", this->name);
					_yield = &ca;
					ETC_SCOPE_EXIT{ _yield = nullptr; };
					hdlr(*this);
				}
			)
		{ ETC_TRACE_CTOR(this->name); }

		~Context()
		{ ETC_TRACE_DTOR(this->name); }

		void yield()
		{
			ETC_TRACE.debug("Context", this, this->name, "yields");
			if (_yield == nullptr)
				throw exception::Exception{"Context not ready"};
			(*_yield)(); // Go back to the scheduler.
			// When we wake up again, we check if we have an exception.
			if (this->exception != nullptr)
			{
				ETC_SCOPE_EXIT{ this->exception = std::exception_ptr{}; };
				std::rethrow_exception(this->exception);
			}
		}
	};

}}

#endif
