#ifndef  CUBE_GL_RENDERER_DRAWABLE_HPP
# define CUBE_GL_RENDERER_DRAWABLE_HPP

# include "fwd.hpp"

namespace cube { namespace gl { namespace renderer {

	class CUBE_API Drawable
	{
	public:
		virtual ~Drawable();

	public:
		virtual void _draw(Painter& p) = 0;
		friend Painter;
	};

}}}

#endif

