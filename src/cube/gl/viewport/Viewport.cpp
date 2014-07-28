#include "Viewport.hpp"

#include <etc/print.hpp>

namespace cube { namespace gl { namespace viewport {

	std::ostream& operator <<(std::ostream& out, Viewport const& vp)
	{
		etc::sprintf(
			out, "Viewport(x=%s, y=%s, w=%s, h=%s)", vp.x, vp.y, vp.w, vp.h
		);
		return out;
	}

}}}
