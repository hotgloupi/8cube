#ifndef  ETC_LOG_HPP
# define ETC_LOG_HPP

# include "log/component.hpp"
# include "log/Logger.hpp"

# include "api.hpp"
# include "types.hpp"
# include "to_string.hpp"
# include "compiler.hpp"

# define ETC_LOG                                                              \
	::etc::log::Log(                                                          \
		::etc::log::Level::info,                                              \
		__FILE__,                                                             \
		__LINE__,                                                             \
		_ETC_LOG_FUNCTION,                                                    \
		etc_log_component()                                                   \
	)                                                                         \
/**/

# define ETC_TRACE                                                            \
	auto BOOST_PP_CAT(log, __LINE__) = ETC_LOG                                \
/**/

# define ETC_TRACE_CTOR(...) ETC_TRACE.debug("Create", *this)(__VA_ARGS__)
# define ETC_TRACE_DTOR(...) ETC_TRACE.debug("Destroy", *this)(__VA_ARGS__)

namespace etc { namespace log {

	struct ETC_API Log
	{
	private:
		Line            _line;
		Logger*         _logger;
		bool            _should_log;
		std::string     _message;

	public:
		/**
		 * Log object should not be built directly, but with macros ETC_LOG*.
		 * They are copiable and movable by construction, but not by assignment.
		 */
		Log(Level const level,
		    char const* file,
		    size_type const line,
		    char const* function,
		    std::string const& component) noexcept;
		Log(Log&& o) noexcept;
		~Log();

	private:
		Log(Log const& o) ETC_DELETED_FUNCTION;
		Log& operator =(Log const&) ETC_DELETED_FUNCTION;
		Log& operator =(Log&&) ETC_DELETED_FUNCTION;

		/**
		 * Send messages with send(...) or operator(...).
		 *
		 * These functions returns the Log object itself (*this) so you can
		 * chain calls.
		 */
	public:
		/// Nothing happens for an empty call.
		inline
		Log&& send() noexcept
		{ return std::move(*this); }

		/// Send values to the logger.
		template<typename... T>
		inline
		Log&& send(T&&... strs) noexcept
		{
			try {
# ifdef NDEBUG // No debug level if NDEBUG is specified.
				if (_line.level > Level::debug)
#endif
				if (_should_log)
				{
					if (_message.size() > 0)
						_message.append(" ");
					_message.append(etc::to_string(std::forward<T>(strs)...));
				}
			} catch (...) { assert(false && "Couldn't log a message"); }
			return std::move(*this);
		}

		/// Forward to send.
		template<typename... T>
		inline
		Log&& operator ()(T&&... strs) noexcept
		{ return this->send(std::forward<T>(strs)...); }

#define _ETC_LOG_LEVEL_PRINTER(lvl)                                           \
		template<typename... T>                                               \
		inline                                                                \
		Log&& lvl(T&&... strs) noexcept                                       \
		{                                                                     \
			assert(_message.size() == 0 || _line.level == Level::lvl);        \
			_line.level = Level::lvl;                                         \
			_should_log = _logger->should_log(_line);                         \
			return this->send(std::forward<T>(strs)...);                      \
		}                                                                     \
	/**/
# ifdef NDEBUG
		template<typename... T>
		inline
		Log&& debug(T const&...) noexcept
		{ return std::move(*this); }
# else
		_ETC_LOG_LEVEL_PRINTER(debug)
# endif
		_ETC_LOG_LEVEL_PRINTER(info)
		_ETC_LOG_LEVEL_PRINTER(warn)
		_ETC_LOG_LEVEL_PRINTER(error)
		_ETC_LOG_LEVEL_PRINTER(fatal)
# undef _ETC_LOG_LEVEL_PRINTER

		inline
		operator bool() const noexcept
		{ return false; }
	};

}}

#endif
