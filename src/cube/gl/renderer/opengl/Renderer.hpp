#ifndef  CUBE_GL_OPENGL_RENDERER_HPP
# define CUBE_GL_OPENGL_RENDERER_HPP

# include "../Renderer.hpp"

namespace cube { namespace gl { namespace renderer { namespace opengl {

	struct RendererType;

	class GLRenderer
		: public renderer::Renderer
	{
	private:
		std::unique_ptr<RendererType> _description;

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
		                     std::vector<std::string> const& sources,
		                     Shader::Parameters inputs,
		                     Shader::Parameters outputs,
		                     Shader::Parameters parameters) override;

		/// Create a shader program from shaders.
		ShaderProgramPtr
		_new_shader_program(std::vector<ShaderPtr>&& shaders) override;

		TexturePtr _new_texture(surface::Surface const& surface) override;

		void draw_elements(renderer::DrawMode mode,
		                   unsigned int count,
		                   cube::gl::renderer::ContentType type,
		                   void* indices);

		void draw_arrays(renderer::DrawMode mode,
		                 etc::size_type start,
		                 etc::size_type count);

		void clear(cube::gl::renderer::BufferBit flags);
		void viewport(cube::gl::viewport::Viewport const& vp);

		void _render_state(RenderState const state, bool const value) override;
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
		void init_versions();
	};

}}}} // !cube::gl::renderer::opengl

#endif
