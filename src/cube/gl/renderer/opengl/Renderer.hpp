#ifndef  CUBE_GL_OPENGL_RENDERER_HPP
# define CUBE_GL_OPENGL_RENDERER_HPP

# include "../Renderer.hpp"

namespace cube { namespace gl { namespace renderer { namespace opengl {

	class GLRenderer
		: public renderer::Renderer
	{
	public:

	public:
		GLRenderer(system::window::RendererContext& context);
		void shutdown();
		cube::gl::renderer::RendererType const& description() const;
		~GLRenderer();
		renderer::Painter begin(renderer::Mode mode);

		/// Create a new vertex buffer.
		VertexBufferPtr
		new_vertex_buffer(std::vector<VertexBufferAttributePtr>&& attributes) override;

		/// Create a new index buffer.
		VertexBufferPtr
		new_index_buffer(VertexBufferAttributePtr&& attributes) override;

	protected:
		ShaderPtr _new_shader(ShaderType const type,
		                     std::vector<std::string> const& sources) override;

		/// Create a shader program from shaders.
		ShaderProgramPtr
		_new_shader_program(std::vector<ShaderPtr>&& shaders) override;

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

	struct RendererType
		: public renderer::RendererType
	{
	public:
		struct { int major, minor; } opengl;
		struct { int major, minor; } glsl;

	public:
		RendererType();
		std::unique_ptr<renderer::Renderer>
		create(system::window::RendererContext& context) override;
		std::string
		__str__() const override;
		Name name() const override { return Name::OpenGL; }
	};

}}}} // !cube::gl::renderer::opengl

#endif
