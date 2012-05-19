#ifndef  CUBE_GL_RENDERERS_OPENGL_IPP
# define CUBE_GL_RENDERERS_OPENGL_IPP

# include <stdexcept>

# include <wrappers/opengl.hpp>

# include "OpenGL.hpp"

namespace cube { namespace gl { namespace renderers {

	namespace detail {

		struct OpenGLType
			: public cube::gl::RendererType
		{
			virtual std::unique_ptr<cube::gl::Renderer> create() const
			{
				std::unique_ptr<cube::gl::Renderer> renderer(new OpenGL);
				renderer->initialize();
				return renderer;
			}
		};

	}

	OpenGL::~OpenGL()
	{
		std::cout << "Destroying opengl renderer\n";
	}

	void OpenGL::initialize()
	{
		std::cout << "Initialize OpenGL renderer" << std::endl
		          << " - OpenGL v" << (char const*)glGetString(GL_VERSION) << std::endl
		          << " - Glew v" << (char const*)glewGetString(GLEW_VERSION) << std::endl;

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

	void OpenGL::shutdown()
	{
		std::cout << "Shutting down OpenGL renderer\n";
	}

	cube::gl::RendererType const& OpenGL::description() const
	{
		static detail::OpenGLType descr;
		return descr;
	}

}}} // !cube::gl::renderers

#endif
