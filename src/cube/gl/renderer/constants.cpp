#include "constants.hpp"

#include <ostream>

namespace cube { namespace gl { namespace renderer {

	std::ostream& operator <<(std::ostream& out, Mode mode)
	{
		switch (mode)
		{
# define _CASE(name)                                                          \
		case Mode::name:                                                      \
			out << "Mode::" #name;                                            \
			break                                                             \
	/**/
			_CASE(none);
			_CASE(_2d);
			_CASE(_3d);
		default:
			out << "Unknown mode";
		}
		return out;
	}

}}}
