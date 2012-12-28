
# include <cube/gl/matrix.hpp>

# include "Renderer.hpp"
# include "VertexBuffer.hpp"
# include "Shader.hpp"
# include "ShaderProgram.hpp"
# include "Texture.hpp"

# include "_opengl.hpp"

# include <cassert>
# include <iostream>
# include <stdexcept>
# include <string>

namespace cube { namespace gl { namespace renderer { namespace opengl {

	ETC_LOG_COMPONENT("cube.gl.renderer.opengl.Renderer");

	namespace detail {

		struct GLRendererType
			: public RendererType
		{
			virtual
			std::unique_ptr<Renderer>
			create(::cube::gl::viewport::Viewport const& vp) const
			{
				std::unique_ptr<Renderer> renderer(new GLRenderer);
				renderer->initialize(vp);
				return renderer;
			}

			virtual
			std::string
			__str__() const
			{
				return etc::to_string(
					"GLEW version", glewGetString(GLEW_VERSION), '\n',
					"OpenGL version", (char*) glGetString(GL_VERSION), '\n',
					"OpenGL renderer", (char*) glGetString(GL_RENDERER), '\n',
					"OpenGL vendor", (char*) glGetString(GL_VENDOR), '\n',
					"GLSL version", (char*) glGetString(GL_SHADING_LANGUAGE_VERSION), '\n'
				);
			}

			virtual
			Name name() const { return Name::OpenGL; }
		};

	} // !detail


	GLRenderer::~GLRenderer()
	{}

	void GLRenderer::initialize(cube::gl::viewport::Viewport const& vp)
	{
		ETC_TRACE.debug("GLRenderer::initialize(", vp, ")");
		::glewExperimental = GL_TRUE;
		auto error = ::glewInit();
		if (error != GLEW_OK)
			throw Exception{
				"Cannot initialize OpenGL renderer: " +
				std::string((char const*) glewGetErrorString(error))
			};

		bool has_required_extensions = (
			  GLEW_ARB_shading_language_100
			& GLEW_ARB_shader_objects
			& GLEW_ARB_vertex_shader
			& GLEW_ARB_fragment_shader
		);
		if (!has_required_extensions)
			throw Exception{"Some required extensions are not available"};
		gl::ClearColor(1.0f, 0, 0, 1.0f);
		this->viewport(vp);
	}

	void GLRenderer::swap_buffers()
	{
		::SDL_GL_SwapBuffers();
	}

	void GLRenderer::shutdown()
	{
		ETC_TRACE.debug("Shutting down GLRenderer renderer");
	}

	RendererType const& GLRenderer::description() const
	{
		static detail::GLRendererType descr;
		return descr;
	}

	renderer::Painter GLRenderer::begin(Mode mode)
	{
		ETC_TRACE.debug("Begin mode", mode);
		State state{mode};

		switch (mode)
		{
		case Mode::_2d:
			gl::Disable(GL_CULL_FACE);
			gl::Disable(GL_DEPTH_TEST);
			gl::Enable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		case Mode::_3d:
			//gl::Enable(GL_DEPTH_TEST);
			//gl::Enable(GL_CULL_FACE);
			gl::Enable(GL_BLEND);
			break;
		default:
			break;
		}
		return this->Renderer::begin(std::move(state));
	}

	void GLRenderer::draw_elements(cube::gl::renderer::DrawMode mode,
	                               unsigned int count,
	                               cube::gl::renderer::ContentType type,
	                               void* indices)
	{
		gl::DrawElements(
			gl::get_draw_mode(mode),
			count,
			gl::get_content_type(type),
			indices
		);
	}


	void GLRenderer::draw_arrays(DrawMode mode,
	                             etc::size_type start,
	                             etc::size_type count)
	{
		gl::DrawArrays(gl::get_draw_mode(mode), start, count);
	}

	VertexBufferPtr
	GLRenderer::new_vertex_buffer(
		std::vector<VertexBufferAttributePtr>&& attributes
	)
	{
		return VertexBufferPtr{new VertexBuffer{std::move(attributes)}};
	}

	VertexBufferPtr
	GLRenderer::new_index_buffer(VertexBufferAttributePtr&& attribute)
	{
		std::vector<VertexBufferAttributePtr> attributes;
		attributes.emplace_back(std::move(attribute));
		return VertexBufferPtr{new IndexBuffer{std::move(attributes)}};
	}

	ShaderPtr
	GLRenderer::new_vertex_shader(std::vector<std::string> const& sources)
	{
		return ShaderPtr{new Shader{renderer::ShaderType::vertex, sources}};
	}

	ShaderPtr
	GLRenderer::new_fragment_shader(std::vector<std::string> const& sources)
	{
		return ShaderPtr{new Shader{renderer::ShaderType::fragment, sources}};
	}

	ShaderProgramPtr
	GLRenderer::new_shader_program(std::vector<ShaderPtr>&& shaders)
	{
		return ShaderProgramPtr{new ShaderProgram{std::move(shaders)}};
	}

	TexturePtr GLRenderer::new_texture(std::string const& path)
	{
		return TexturePtr{new Texture{path}};
	}

	TexturePtr
	GLRenderer::new_texture(renderer::PixelFormat const internal_format,
	                        unsigned int width,
	                        unsigned int height,
	                        renderer::PixelFormat const data_format,
	                        renderer::ContentPacking const data_packing,
	                        void const* data)
	{
		return TexturePtr{
			new Texture{
				internal_format,
				width,
				height,
				data_format,
				data_packing,
				data
			}
		};
	}

	void GLRenderer::clear(cube::gl::renderer::BufferBit flags)
	{
		using namespace cube::gl::renderer;
		gl::Clear(
			((flags & BufferBit::color)  != BufferBit::none ? GL_COLOR_BUFFER_BIT : 0) |
			((flags & BufferBit::depth) != BufferBit::none ? GL_DEPTH_BUFFER_BIT : 0) |
			((flags & BufferBit::stencil) != BufferBit::none ? GL_STENCIL_BUFFER_BIT : 0)
		);
	}

	void GLRenderer::viewport(cube::gl::viewport::Viewport const& vp)
	{
		this->Renderer::viewport(vp);
		gl::Viewport(vp.x, vp.y, vp.w, vp.h);
	}

}}}} // !cube::gl::renderers
