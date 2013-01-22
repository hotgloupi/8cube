#include "window.hpp"
#include "../Exception.hpp"
#include "../inputs.hpp"

#include <etc/log.hpp>

#ifdef Expose
# error "Xlib already included"
#endif

#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/glx.h>

namespace cube { namespace system { namespace glx { namespace window {

	ETC_LOG_COMPONENT("cube.system.glx.window.Window");

	typedef cube::system::Exception Exception;

	struct Window::Impl
	{
		::Display*                  display;
		::Window                    root;
		::XVisualInfo*              visual_info;
		::Colormap                  colormap;
		::XSetWindowAttributes      set_window_attributes;
		::Window                    window;
		::GLXContext                gl_context;
		bool                        double_buffered;

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
		, _glx_impl{new Impl{}}
	{
		if ((_glx_impl->display = ::XOpenDisplay(0)) == nullptr)
			throw Exception("Cannot connect to the X server");
		_glx_impl->root = DefaultRootWindow(_glx_impl->display);

		if (!::glXQueryExtension(_glx_impl->display, nullptr, nullptr))
			throw Exception{"X Server has no OpenGL GLX extension"};

		static ::GLint double_buffered_attrs[] = {
			GLX_RGBA,
			GLX_RED_SIZE, 8,
			GLX_GREEN_SIZE, 8,
			GLX_BLUE_SIZE, 8,
			GLX_DEPTH_SIZE, 16,
			GLX_DOUBLEBUFFER,
			None,
		};

		static ::GLint single_buffered_attrs[] = {
			GLX_RGBA,
			GLX_RED_SIZE, 8,
			GLX_GREEN_SIZE, 8,
			GLX_BLUE_SIZE, 8,
			GLX_DEPTH_SIZE, 16,
			GLX_DOUBLEBUFFER,
			None,
		};

		_glx_impl->visual_info = ::glXChooseVisual(
			_glx_impl->display, 0, double_buffered_attrs
		);
		if (_glx_impl->visual_info != nullptr)
		{
			_glx_impl->double_buffered = true;
		}
		else
		{
			_glx_impl->double_buffered = false;
			_glx_impl->visual_info = ::glXChooseVisual(
				_glx_impl->display, 0, single_buffered_attrs
			);
		}

		if (_glx_impl->visual_info == nullptr)
			throw Exception{"Cannot choose GLX visual info"};

		_glx_impl->colormap = ::XCreateColormap(
			_glx_impl->display,
			_glx_impl->root,
			_glx_impl->visual_info->visual,
			AllocNone
		);

		_glx_impl->set_window_attributes.colormap = _glx_impl->colormap;
		_glx_impl->set_window_attributes.event_mask =
			ExposureMask |
			KeyPressMask |
			StructureNotifyMask | //ResizeRedirectMask |
			ButtonReleaseMask | ButtonPressMask |
			PointerMotionMask;
		_glx_impl->window = ::XCreateWindow(
			_glx_impl->display,
			_glx_impl->root,
			40, 40, width, height,
			0, _glx_impl->visual_info->depth,
			InputOutput,
			_glx_impl->visual_info->visual,
			CWColormap | CWEventMask,
			&_glx_impl->set_window_attributes
		);

		::XMapWindow(_glx_impl->display, _glx_impl->window);
		::XStoreName(_glx_impl->display, _glx_impl->window, title.c_str());
		_glx_impl->gl_context = glXCreateContext(
			_glx_impl->display,
			_glx_impl->visual_info,
			NULL, GL_TRUE
		);
		::glXMakeCurrent(
			_glx_impl->display,
			_glx_impl->window,
			_glx_impl->gl_context
		);
		Atom wmDelete = ::XInternAtom(
			_glx_impl->display, "WM_DELETE_WINDOW", True
		);
		::XSetWMProtocols(
			_glx_impl->display, _glx_impl->window, &wmDelete, 1
		);
	}

	Window::~Window()
	{
		::glXMakeCurrent(_glx_impl->display, None, NULL);
		::glXDestroyContext(_glx_impl->display, _glx_impl->gl_context);
		::XDestroyWindow(_glx_impl->display, _glx_impl->window);
		::XCloseDisplay(_glx_impl->display);
	}

	etc::size_type Window::poll(etc::size_type const max)
	{
		bool has_expose = false;
		bool has_resize = false;

		size_t count = 0;
		XEvent ev;
		XConfigureEvent config_ev;
		while (count < max && ::XPending(_glx_impl->display) > 0)
		{
			XNextEvent(_glx_impl->display, &ev);

			switch (ev.type)
			{
			case Expose:
				has_expose = true;
				break;

			case KeyPress:
				//this->inputs().on_keydown()(ev.xkey.keycode, ev.xkey.state);
				break;

			case KeyRelease:
				//this->inputs().on_keyup()(ev.xkey.keycode, ev.xkey.state);
				break;

			case ClientMessage: //ev.type == DestroyNotify ||
				this->inputs().on_quit()();
				break;

			case ButtonRelease:
				break;

			case MotionNotify:
				this->inputs().on_mousemove()(ev.xmotion.x, ev.xmotion.y);
				break;

			case ConfigureNotify:
				config_ev = ev.xconfigure;
				if (config_ev.width == 0 || config_ev.height == 0)
					continue;
				_width = config_ev.width;
				_height = config_ev.height;
				has_resize = true;
				break;

			default:
				ETC_LOG.warn("Ignored event:", ev.type);
				break;
			}
			count++;
		}

		if (has_resize)
		{
			ETC_TRACE.debug("Got resize event", _width, _height);
			this->inputs().on_resize()(_width, _height);
		}

		if (has_expose)
		{
			ETC_TRACE.debug("Got expose event");
			this->inputs().on_expose()(_width, _height);
		}

		if (count == 0)
			this->inputs().on_idle()();

		return count;
	}

	void Window::swap_buffers()
	{
		if (_glx_impl->double_buffered)
			::glXSwapBuffers(_glx_impl->display, _glx_impl->window);
	}

	void Window::confine_mouse(bool mode)
	{

	}

}}}}

