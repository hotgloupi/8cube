#ifndef CUBE_GL_VIEWPORT_HPP
# define CUBE_GL_VIEWPORT_HPP

# include "rectangle.hpp"

# include <cube/api.hpp>

namespace cube { namespace gl { namespace viewport {

	struct CUBE_API Viewport
		: public rectangle::Rectangle2f
	{
		inline
		Viewport(float x, float y, float w, float h) ETC_NOEXCEPT
			: rectangle::Rectangle2f{x, y, w, h}
		{}

		inline
		Viewport(Viewport const& other) ETC_NOEXCEPT
			: rectangle::Rectangle2f{other}
		{}

		inline
		Viewport& operator =(Viewport const& other) ETC_NOEXCEPT
		{
			this->rectangle::Rectangle2f::operator =(other);
			return *this;
		}
	};

	CUBE_API
	std::ostream& operator <<(std::ostream& out, Viewport const& vp);

}}}

#endif /* ! VIEWPORT_HPP */


