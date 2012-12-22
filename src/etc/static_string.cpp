#ifndef _WIN32 //XXX
#include "static_string.hpp"

#include <iostream>

namespace etc { namespace detail {

	std::ostream& print_string(std::ostream& out,
	                           char const* data,
	                           unsigned int size)
	{
		return out.write(data, size);
	}

}}
#endif
