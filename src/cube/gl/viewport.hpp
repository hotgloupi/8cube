#ifndef CUBE_GL_VIEWPORT_HPP
# define CUBE_GL_VIEWPORT_HPP

# include "rectangle.hpp"

# include <cube/api.hpp>

namespace cube { namespace gl { namespace viewport {

	struct CUBE_API Viewport
		: public cube::gl::rectangle::Rectangle2f
	{
		Viewport(float x, float y, float w, float h)
			: cube::gl::rectangle::Rectangle2f{x, y, w, h}
		{}

		inline
		Viewport& operator =(Viewport const& other)
		{
			this->cube::gl::rectangle::Rectangle2f::operator =(other);
			return *this;
		}
	};

	CUBE_API
	std::ostream& operator <<(std::ostream& out, Viewport const& vp);

}}}

#endif /* ! VIEWPORT_HPP */


