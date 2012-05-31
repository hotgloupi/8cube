#ifndef  CUBE_GL_RENDERERS_OPENGL_HPP
# define CUBE_GL_RENDERERS_OPENGL_HPP

# include <iostream>

# include <cube/gl/renderer.hpp>

namespace cube { namespace gl { namespace renderers {

	class OpenGL
		: public cube::gl::renderer::Renderer
	{
	public:

	public:
		virtual void initialize(cube::gl::viewport::Viewport const& vp);
		virtual void shutdown();
		virtual void swap_buffers();
		virtual cube::gl::renderer::RendererType const& description() const;
		virtual ~OpenGL();
		virtual Painter begin(Mode mode);
		virtual VertexBufferPtr new_vertex_buffer();
	protected:
		virtual void end();
	};

}}} // !cube::gl::renderers

#endif
