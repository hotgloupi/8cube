#ifndef CUBE_GL_VIEWPORT_HPP
# define CUBE_GL_VIEWPORT_HPP

# include "rectangle.hpp"

namespace cube { namespace gl { namespace viewport {

	struct Viewport : cube::gl::rectangle::Rectangle2f
	{
		Viewport(float x, float y, float w, float h)
			: cube::gl::rectangle::Rectangle2f{x, y, w, h}
		{}

		Viewport& operator =(Viewport const& other)
		{
			if (this != &other)
			{
				this->cube::gl::rectangle::Rectangle2f::operator =(other);
			}
			return *this;
		}
	};

}}}

#endif /* ! VIEWPORT_HPP */


