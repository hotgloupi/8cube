#include "cube.hpp"

#include <iostream>

namespace cube { namespace gl { namespace cube {

	template<typename T>
	std::ostream& operator <<(std::ostream& out, Cube3<T> const& cube)
	{
		out << "<Cube3 center=" << cube.center
		    << " width=" << cube.width << '>';
		return out;
	}

#define SPECIALIZE_OSTREAM(__type)                                            \
	template                                                                  \
	std::ostream& operator <<<__type>(std::ostream&, Cube3<__type> const&)    \
/**/

	SPECIALIZE_OSTREAM(float);
	SPECIALIZE_OSTREAM(double);
	SPECIALIZE_OSTREAM(int32_t);
	SPECIALIZE_OSTREAM(uint32_t);
	SPECIALIZE_OSTREAM(int64_t);
	SPECIALIZE_OSTREAM(uint64_t);

}}}