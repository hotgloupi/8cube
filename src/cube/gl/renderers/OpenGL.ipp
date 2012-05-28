#ifndef  CUBE_GL_RENDERERS_OPENGL_IPP
# define CUBE_GL_RENDERERS_OPENGL_IPP

# include <cassert>
# include <stdexcept>
# include <string>

# include <boost/type_traits.hpp>

# include <wrappers/sdl.hpp>
# include <wrappers/opengl.hpp>

# include <cube/gl/matrix.hpp>

# include "OpenGL.hpp"

namespace cube { namespace gl { namespace renderers {

	namespace detail {

		struct OpenGLType
			: public cube::gl::renderer::RendererType
		{
			std::unique_ptr<cube::gl::renderer::Renderer>
				create(::cube::gl::viewport::Viewport const& vp) const
			{
				std::unique_ptr<cube::gl::renderer::Renderer> renderer(new OpenGL);
				renderer->initialize(vp);
				return renderer;
			}

			std::string
				__str__() const
			{
				std::string gl_version = (char const*)glGetString(GL_VERSION);
				std::string glew_version = (char const*)glewGetString(GLEW_VERSION);
				return (
					"OpenGL v" + gl_version + "\n"
					"GLEW v" + glew_version + "\n"
				);
			}
		};

	}

	namespace {

		struct gl
		{
# define GLWRAP(__name__)                                                     \
			typedef                                                           \
				boost::function_traits<decltype(glEnable)>::result_type       \
				__name__ ## _result_type;                                     \
			template<typename... T>                                           \
			static inline __name__ ## _result_type                            \
			__name__(T... values)                                             \
			{                                                                 \
				return ::gl ## __name__(values...);                           \
			}                                                                 \

			GLWRAP(Enable);
			GLWRAP(Disable);

# undef GLWRAP
		};

	} // !anonymous

	OpenGL::~OpenGL()
	{
		std::cout << "Destroying opengl renderer\n";
	}

	void OpenGL::initialize(cube::gl::viewport::Viewport const& vp)
	{
		auto error = glewInit();
		if (error != GLEW_OK)
			throw std::runtime_error(
				"Cannot initialize OpenGL renderer: " +
				std::string((char const*) glewGetErrorString(error))
			);
		if (!(GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader))
			throw std::runtime_error(
				"Cannot start OpenGL"
				//"ARB_vertex_shader and ARB_fragment_shader are required"
			);
		_viewport = vp;
	}

	void OpenGL::swap_buffers()
	{
        ::SDL_GL_SwapBuffers();
	}

	void OpenGL::shutdown()
	{
		std::cout << "Shutting down OpenGL renderer\n";
	}

	cube::gl::renderer::RendererType const& OpenGL::description() const
	{
		static detail::OpenGLType descr;
		return descr;
	}

	OpenGL::Painter OpenGL::begin(Mode mode)
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

	void OpenGL::end()
	{

	}

}}} // !cube::gl::renderers

#endif
