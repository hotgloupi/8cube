#ifndef  CUBE_GL_RENDERERS_OPENGL_HPP
# define CUBE_GL_RENDERERS_OPENGL_HPP

# include <iostream>

# include <cube/gl/renderer.hpp>

namespace cube { namespace gl { namespace renderers {

	class OpenGL
		: public cube::gl::Renderer
	{
	public:

	public:
		virtual void initialize();
		virtual void shutdown();
		virtual void swap_buffers();
		virtual cube::gl::RendererType const& description() const;
		virtual ~OpenGL();
	};

}}} // !cube::gl::renderers

#endif
