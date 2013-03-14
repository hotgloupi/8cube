#ifndef  CUBE_GL_RENDERER_DRAWABLE_HPP
# define CUBE_GL_RENDERER_DRAWABLE_HPP

namespace cube { namespace gl { namespace renderer {

	class Drawable
	{
	public:
		virtual ~Drawable();

	protected:
		virtual void _draw(Painter& p) = 0;
	};

}}}

#endif

