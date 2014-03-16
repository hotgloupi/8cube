#include "constants.hpp"

#include <iostream>

namespace etc { namespace http {

	std::ostream& operator <<(std::ostream& out, Method const method)
	{
		switch (method)
		{
#define CASE(__name) \
		case Method::__name: \
			return out << "Method::" #__name \
/**/
			CASE(get);
			CASE(put);
			CASE(post);
			CASE(delete_);
			CASE(head);
#undef CASE

		case Method::_max_value:
		default:
			return out << "Method::INVALID (" << (int) method << ")";
		}
	}

	std::ostream& operator <<(std::ostream& out, ResponseCode const code)
	{
		switch (code)
		{
#define CASE(__name) \
		case ResponseCode::__name: \
			return out << "ResponseCode::" #__name << " (" << (int) code << ")"\
/**/
			CASE(ok);
			CASE(not_found);
#undef CASE

		case ResponseCode::_max_value:
		default:
			return out << "ResponseCode::INVALID (" << (int) code << ")";
		}

	}
}}
