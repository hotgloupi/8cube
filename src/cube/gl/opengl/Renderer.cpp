
# include <cube/gl/matrix.hpp>

# include "Renderer.hpp"
# include "VertexBuffer.hpp"
# include "Shader.hpp"
# include "ShaderProgram.hpp"

# include "_opengl.hpp"

# include <cassert>
# include <iostream>
# include <stdexcept>
# include <string>

namespace cube { namespace gl { namespace opengl {

	using namespace cube::gl::renderer;

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
					"OpenGL vendor", (char*) glGetString(GL_VENDOR), '\n'
				);
			}

			virtual
			Name name() const { return Name::OpenGL; }
		};

	} // !detail


	GLRenderer::~GLRenderer()
	{
		std::cerr << "Destroying opengl renderer\n";
	}

	void GLRenderer::initialize(cube::gl::viewport::Viewport const& vp)
	{
		etc::log::debug("GLRenderer::initialize(", vp.x, vp.y, vp.w, vp.h, ")");
		::glewExperimental = GL_TRUE;
		auto error = ::glewInit();
		if (error != GLEW_OK)
			throw std::runtime_error{
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
		std::cout << "Shutting down GLRenderer renderer\n";
	}

	RendererType const& GLRenderer::description() const
	{
		static detail::GLRendererType descr;
		return descr;
	}

	GLRenderer::Painter GLRenderer::begin(Mode mode)
	{
		float width = _viewport.w - _viewport.x;
		float height = _viewport.h - _viewport.y;
		matrix_type translate2d = matrix::translate<float>(
			width / 2.0f , height / 2.0f, 0.0f
		);

		State state{mode};

		switch (mode)
		{
		case Mode::none:
			break;
		case Mode::_2d:
			gl::Disable(GL_CULL_FACE);
			gl::Disable(GL_DEPTH_TEST);
			//auto size = rs.target == 0 ? this->_screenSize : rs.target->GetSize();
			//state.view = translate2d,
			state.projection(::cube::gl::matrix::ortho<float>(
				0.0f, width,
				height, 0.0f
			));
			break;
		case Mode::_3d:
			gl::Enable(GL_DEPTH_TEST);
			gl::Enable(GL_CULL_FACE);
			break;
		default:
			assert(false && "Unknown mode");
		}
		return this->Renderer::begin(state);
	}

	void GLRenderer::draw_elements(cube::gl::renderer::DrawMode mode,
	                               unsigned int count,
	                               cube::gl::renderer::ContentType type,
	                               void* indices)
	{
		etc::log::debug(
			__PRETTY_FUNCTION__,
			(int) mode,
			count,
			(int) type,
			indices
		);

		gl::DrawElements(
			gl::get_draw_mode(mode),
			count,
			gl::get_content_type(type),
			indices
		);
	}

	GLRenderer::VertexBufferPtr
	GLRenderer::new_vertex_buffer()
	{
		return VertexBufferPtr{new GLVertexBuffer};
	}

	GLRenderer::VertexBufferPtr
	GLRenderer::new_index_buffer()
	{
		return VertexBufferPtr{new GLIndexBuffer};
	}

	GLRenderer::ShaderPtr GLRenderer::new_vertex_shader()
	{
		return ShaderPtr{new Shader{renderer::ShaderType::vertex}};
	}

	GLRenderer::ShaderPtr GLRenderer::new_fragment_shader()
	{
		return ShaderPtr{new Shader{renderer::ShaderType::fragment}};
	}

	GLRenderer::ShaderProgramPtr GLRenderer::new_shader_program()
	{
		return ShaderProgramPtr{new ShaderProgram};
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

}}} // !cube::gl::renderers
