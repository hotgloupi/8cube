#ifndef  CUBE_GL_RENDERER_DRAWABLE_HPP
# define CUBE_GL_RENDERER_DRAWABLE_HPP

# include <cube/gl/fwd.hpp>

namespace cube { namespace gl { namespace renderer {

	class Drawable
	{
	public:
		virtual ~Drawable() {}
	protected:
		virtual void _bind() = 0;
		virtual void _unbind() = 0;
		friend class Renderer;
	};

}}}

#endif

