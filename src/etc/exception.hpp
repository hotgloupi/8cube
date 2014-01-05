#ifndef  ETC_EXCEPTION_HPP
# define ETC_EXCEPTION_HPP

# include "api.hpp"
# include "fwd.hpp"

# include <string>
# include <stdexcept>

namespace etc { namespace exception {

	std::string string();
	std::string string(std::exception_ptr e);

	class ETC_API_EXCEPTION Exception
		: public std::runtime_error
	{
	private:
		backtrace::Backtrace*   _backtrace;
		std::string             _msg;

	public:
		Exception(std::string const& message);

		virtual
		~Exception() throw ();

		backtrace::Backtrace const*
		backtrace() const { return _backtrace; }

		friend
		ETC_API std::ostream& operator <<(std::ostream& out, Exception const& e);
	};

	ETC_API std::ostream& operator <<(std::ostream& out, Exception const& e);

}} // !etc::exception

# include <boost/preprocessor/control/if.hpp>
# include <boost/preprocessor/arithmetic/dec.hpp>
# include <boost/preprocessor/variadic/size.hpp>

/**
 * @brief Defines an exception class Name.
 *
 * @param Name  The class name
 * @param ...   Parent exception class (defaults to etc::exception::Exception)
 *
 * Note: You have to add a trailing semicolon (;).
 */
# define ETC_EXCEPTION_CLASS(Name, ...)                                       \
	class Name                                                                \
		: public BOOST_PP_IF(                                                 \
					BOOST_PP_DEC(BOOST_PP_VARIADIC_SIZE(Name, ##__VA_ARGS__)),\
					__VA_ARGS__,                                              \
					::etc::exception::Exception                               \
				)                                                             \
	{                                                                         \
	public:                                                                   \
		Name(std::string const& msg)                                          \
			: BOOST_PP_IF(                                                    \
					BOOST_PP_DEC(BOOST_PP_VARIADIC_SIZE(Name, ##__VA_ARGS__)),\
					__VA_ARGS__,                                              \
					::etc::exception::Exception                               \
				){msg}                                                        \
		{}                                                                    \
	}                                                                         \
/**/

#endif
