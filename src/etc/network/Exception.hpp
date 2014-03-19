#ifndef ETC_NETWORK_EXCEPTION_HPP
# define ETC_NETWORK_EXCEPTION_HPP

# include <etc/exception.hpp>
# include <etc/to_string.hpp>

# include <system_error>

namespace etc { namespace network {

	using exception::Exception;

	class InvalidOperation
		: public Exception
	{
	public:
		InvalidOperation()
			: Exception{"Invalid operation"}
		{}
	};

	class SystemError
		: public Exception
	{
	private:
		std::error_code _error_code;
	public:
		SystemError(std::string msg, std::error_code const& ec)
			: Exception(msg + ": " + ec.message() + " (" + etc::to_string(ec) + ")")
			, _error_code{ec}
		{}

	};


}}

#endif
