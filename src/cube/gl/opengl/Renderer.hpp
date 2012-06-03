#ifndef  CUBE_GL_OPENGL_RENDERER_HPP
# define CUBE_GL_OPENGL_RENDERER_HPP

# include <cube/gl/renderer.hpp>

namespace cube { namespace gl { namespace opengl {

	class GLRenderer
		: public cube::gl::renderer::Renderer
	{
	public:

	public:
		virtual void initialize(cube::gl::viewport::Viewport const& vp);
		virtual void shutdown();
		virtual void swap_buffers();
		virtual cube::gl::renderer::RendererType const& description() const;
		virtual ~GLRenderer();
		virtual Painter begin(Mode mode);
		virtual VertexBufferPtr new_vertex_buffer();
		virtual void draw_elements(cube::gl::renderer::DrawMode mode,
		                           unsigned int count,
		                           cube::gl::renderer::ContentType type,
		                           void* indices);
	protected:
		virtual void _end();
	};

}}} // !cube::gl::opengl

#endif
