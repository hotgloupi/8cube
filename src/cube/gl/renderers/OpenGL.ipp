#ifndef  CUBE_GL_RENDERERS_OPENGL_IPP
# define CUBE_GL_RENDERERS_OPENGL_IPP

# include <stdexcept>
# include <string>

# include <wrappers/sdl.hpp>
# include <wrappers/opengl.hpp>

# include "OpenGL.hpp"

namespace cube { namespace gl { namespace renderers {

	namespace detail {

		struct OpenGLType
			: public cube::gl::renderer::RendererType
		{
			virtual std::unique_ptr<cube::gl::renderer::Renderer> create() const
			{
				std::unique_ptr<cube::gl::renderer::Renderer> renderer(new OpenGL);
				renderer->initialize();
				return renderer;
			}
			virtual std::string __str__() const
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

	OpenGL::~OpenGL()
	{
		std::cout << "Destroying opengl renderer\n";
	}

	void OpenGL::initialize()
	{
		auto error = glewInit();
		if (error != GLEW_OK)
			throw std::runtime_error(
				"Cannot initialize OpenGL renderer: " +
				std::string((char const*) glewGetErrorString(error))
			);
		if (!(GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader))
			throw std::runtime_error(
				"ARB_vertex_shader and ARB_fragment_shader are required"
			);
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

}}} // !cube::gl::renderers

#endif
