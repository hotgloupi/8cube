#ifdef CUBE_WITH_OPENGL_ES

# include "window.hpp"

# include <GLES2/gl2.h>
# include <EGL/elg.h>

namespace cube { namespace system { namespace glx { namespace window {

	struct Window::Impl
	{
		EGLDisplay display;

		Impl()
		{
			bzero(this, sizeof(*this));
		}
	};

	Window::Window(std::string const& title,
	               etc::size_type const width,
	               etc::size_type const height,
	               gl::renderer::Name const renderer_name)
		: cube::system::window::Window{title, width, height, renderer_name}
		, _impl{new Impl}
	{

	}

	Window::~Window()
	{

	}

	etc::size_type Window::poll(etc::size_type const max)
	{

	}

	void Window::confine_mouse(bool mode)
	{

	}

	void Window::swap_buffers()
	{

	}

}}}}

#endif // !CUBE_WITH_OPENGL_ES
