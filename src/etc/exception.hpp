#ifndef  ETC_EXCEPTION_HPP
# define ETC_EXCEPTION_HPP

# include "fwd.hpp"

# include <string>
# include <stdexcept>

namespace etc { namespace exception {

	class Exception
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

		friend std::ostream& operator <<(std::ostream& out, Exception const& e);
	};

	std::ostream& operator <<(std::ostream& out, Exception const& e);

}} // !etc::exception

#endif
