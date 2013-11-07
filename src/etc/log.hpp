#ifndef  ETC_LOG_HPP
# define ETC_LOG_HPP

# include "api.hpp"
# include "log/Logger.hpp"
# include "types.hpp"
# include "compiler.hpp"

# include <boost/preprocessor/cat.hpp>

# define ETC_LOG_COMPONENT(__name)                                            \
	static constexpr char const* _etc_log_component = __name;                 \
/**/
# define ETC_LOG_SUB_COMPONENT(__name)                                        \
	static auto BOOST_PP_CAT(log, __LINE__) =                                 \
		std::string{_etc_log_component} + "#" __name;                         \
	static char const* _etc_log_component =                                   \
		BOOST_PP_CAT(log, __LINE__).c_str()                                   \
/**/

# define ETC_LOG                                                              \
	::etc::log::Log(                                                          \
		::etc::log::Level::info,                                              \
		__FILE__,                                                             \
		__LINE__,                                                             \
		_ETC_LOG_FUNCTION,                                                    \
		_etc_log_component                                                    \
	)                                                                         \
/**/

# define ETC_TRACE                                                            \
	auto BOOST_PP_CAT(log, __LINE__) = ETC_LOG                                \
/**/

namespace etc { namespace log {

	struct ETC_API Log
	{
	private:
		Line            _line;
		Logger&         _logger;
		bool            _dtor_indent;

	public:
		/**
		 * Log object should not be built directly, but with macros ETC_LOG*.
		 * They are copiable and movable by construction, but not by assignment.
		 */
		ETC_API Log(Level level,
		            char const* file,
		            size_type line,
		            char const* function,
		            char const* component);
		ETC_API Log(Log const& o);
		ETC_API Log(Log&& o);
		ETC_API ~Log();

	private:
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
		Log& send() noexcept
		{ return *this; }

		/// Send values to the logger.
		template<typename... T>
		Log& send(T const&... strs)
		{
# ifdef NDEBUG
			// No debug level if NDEBUG is specified.
			if (_line.level <= Level::debug)
				return *this;
#endif
			_logger.message(_line, strs...);
			return *this;
		}

		/// Forward to send.
		template<typename... T>
		Log& operator ()(T const&... strs)
		{
			return this->send(strs...);
		}

#define _ETC_LOG_LEVEL_PRINTER(lvl)                                           \
		template<typename... T>                                               \
		Log lvl(T const&... strs)                                             \
		{                                                                     \
			Log res{                                                          \
				Level::lvl,                                                   \
				_line.file, _line.line,                                       \
				_line.function, _line.component                               \
			};                                                                \
			res.send(                                                         \
				strs...                                                       \
			);                                                                \
			return res;                                                       \
		}                                                                     \
	/**/
# ifdef NDEBUG
		template<typename... T> inline Log debug(T const&...)
		{
			return Log{
				Level::debug,
				_line.file, _line.line,
				_line.function, _line.component
			};
		}
# else
		_ETC_LOG_LEVEL_PRINTER(debug)
# endif
		_ETC_LOG_LEVEL_PRINTER(info)
		_ETC_LOG_LEVEL_PRINTER(warn)
		_ETC_LOG_LEVEL_PRINTER(error)
		_ETC_LOG_LEVEL_PRINTER(fatal)
# undef _ETC_LOG_LEVEL_PRINTER

		operator bool() const noexcept {return false;}

	private:
		static
		etc::size_type _current_indent();
	};

	struct Indent
	{
		private:
			static size_type _indent;
		public:
			static inline size_type increment() { return ++_indent; }
			static inline size_type decrement() { return --_indent; }
			static inline size_type get() { return _indent; }
	};

	extern Indent indent;

}}

#endif
