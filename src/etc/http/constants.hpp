#ifndef ETC_HTTP_CONSTANTS_HPP
# define ETC_HTTP_CONSTANTS_HPP

# include <iosfwd>

namespace etc { namespace http {

	enum class Method
	{
		get, put, post, delete_, head,

		_max_value
	};

	std::ostream& operator <<(std::ostream& out, Method const method);

	enum class ResponseCode
	{
		ok = 200,
		not_found = 404,

		_max_value
	};

	std::ostream& operator <<(std::ostream& out, ResponseCode const code);
}}

#endif
