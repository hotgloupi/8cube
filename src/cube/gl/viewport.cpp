#include "viewport.hpp"

#include <etc/print.hpp>

namespace cube { namespace gl { namespace viewport {

	std::ostream& operator <<(std::ostream& out, Viewport const& vp)
	{
		etc::sprintf(
			out, "Viewport(%s, %s, %s, %s)", vp.x, vp.y, vp.w, vp.h
		);
		return out;
	}

}}}
