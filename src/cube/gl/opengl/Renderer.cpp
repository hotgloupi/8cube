# include <cassert>
# include <iostream>
# include <stdexcept>
# include <string>

# include <cube/gl/matrix.hpp>

# include "Renderer.hpp"
# include "VertexBuffer.hpp"

# include "_opengl.hpp"

namespace cube { namespace gl { namespace opengl {

	using namespace cube::gl::renderer;

	namespace detail {

		struct GLRendererType
			: public RendererType
		{
			std::unique_ptr<Renderer>
				create(::cube::gl::viewport::Viewport const& vp) const
			{
				std::unique_ptr<Renderer> renderer(new GLRenderer);
				renderer->initialize(vp);
				return renderer;
			}

			std::string
				__str__() const
			{
				std::string gl_version = (char const*)glGetString(GL_VERSION);
				std::string glew_version = (char const*)glewGetString(GLEW_VERSION);
				return (
					"GLRenderer v" + gl_version + "\n"
					"GLEW v" + glew_version + "\n"
				);
			}
		};

	}


	GLRenderer::~GLRenderer()
	{
		std::cout << "Destroying opengl renderer\n";
	}

	void GLRenderer::initialize(cube::gl::viewport::Viewport const& vp)
	{
		auto error = glewInit();
		if (error != GLEW_OK)
			throw std::runtime_error(
				"Cannot initialize GLRenderer renderer: " +
				std::string((char const*) glewGetErrorString(error))
			);
		if (!(GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader))
			throw std::runtime_error(
				"Cannot start GLRenderer"
				//"ARB_vertex_shader and ARB_fragment_shader are required"
			);
		_viewport = vp;
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
		static matrix_type translate2d = ::cube::gl::matrix::translate<float>(0, 0, 1);

		State state;
		state.mode = mode;

		switch (mode)
		{
		case Mode::none:
			break;
		case Mode::_2d:
			gl::Disable(GL_CULL_FACE);
			gl::Disable(GL_DEPTH_TEST);
			//auto size = rs.target == 0 ? this->_screenSize : rs.target->GetSize();
			state.view = translate2d,
			state.projection = ::cube::gl::matrix::ortho<float>(
				0, _viewport.w - _viewport.x,
				_viewport.h - _viewport.y, 0
			);
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

	void GLRenderer::_end()
	{

	}

	GLRenderer::VertexBufferPtr GLRenderer::new_vertex_buffer()
	{
		return VertexBufferPtr{new GLVertexBuffer};
	}

}}} // !cube::gl::renderers

