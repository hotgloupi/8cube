#ifndef  CUBE_GL_OPENGL_RENDERER_HPP
# define CUBE_GL_OPENGL_RENDERER_HPP

# include "../Renderer.hpp"

namespace cube { namespace gl { namespace renderer { namespace opengl {

	class GLRenderer
		: public renderer::Renderer
	{
	public:

	public:
		void initialize(cube::gl::viewport::Viewport const& vp);
		void shutdown();
		void swap_buffers();
		cube::gl::renderer::RendererType const& description() const;
		~GLRenderer();
		renderer::Painter begin(renderer::Mode mode);
		VertexBufferPtr new_vertex_buffer();
		VertexBufferPtr new_index_buffer();
		ShaderPtr new_vertex_shader();
		ShaderPtr new_fragment_shader();
		ShaderProgramPtr new_shader_program();
		TexturePtr new_texture(std::string const& path);

		TexturePtr new_texture(renderer::PixelFormat const internal_format,
		                       unsigned int width,
		                       unsigned int height,
		                       renderer::PixelFormat const data_format,
		                       renderer::ContentPacking const data_packing,
		                       void const* data);
		void draw_elements(renderer::DrawMode mode,
		                   unsigned int count,
		                   cube::gl::renderer::ContentType type,
		                   void* indices);
		void draw_arrays(renderer::DrawMode mode,
		                 etc::size_type start,
		                 etc::size_type count);
		void clear(cube::gl::renderer::BufferBit flags);
		void viewport(cube::gl::viewport::Viewport const& vp);
	};

}}}} // !cube::gl::renderer::opengl

#endif
