#ifndef  CUBE_GL_OPENGL_RENDERER_HPP
# define CUBE_GL_OPENGL_RENDERER_HPP

# include "../Renderer.hpp"

namespace cube { namespace gl { namespace renderer { namespace opengl {

	class GLRenderer
		: public renderer::Renderer
	{
	public:

	public:
		virtual void initialize(cube::gl::viewport::Viewport const& vp);
		virtual void shutdown();
		virtual void swap_buffers();
		virtual cube::gl::renderer::RendererType const& description() const;
		virtual ~GLRenderer();
		virtual renderer::Painter begin(renderer::Mode mode);
		virtual VertexBufferPtr new_vertex_buffer();
		virtual VertexBufferPtr new_index_buffer();
		virtual ShaderPtr new_vertex_shader();
		virtual ShaderPtr new_fragment_shader();
		virtual ShaderProgramPtr new_shader_program();
		virtual TexturePtr new_texture(std::string const& path);
		virtual void draw_elements(cube::gl::renderer::DrawMode mode,
		                           unsigned int count,
		                           cube::gl::renderer::ContentType type,
		                           void* indices);
		virtual void clear(cube::gl::renderer::BufferBit flags);
		virtual void viewport(cube::gl::viewport::Viewport const& vp);
	};

}}}} // !cube::gl::renderer::opengl

#endif
