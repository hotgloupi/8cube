#ifndef ETC_HTTP_CONSTANTS_HPP
# define ETC_HTTP_CONSTANTS_HPP

namespace etc { namespace http {

	enum class Method
	{
		get, put, post, delete_, head,

		_max_value
	};

	enum class ResponseCode
	{
		ok = 200,
	};

}}

#endif
