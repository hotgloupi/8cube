#ifndef  CUBE_SYSTEM_WINDOW_IPP
# define CUBE_SYSTEM_WINDOW_IPP

# include <wrappers/sdl.hpp>

# include <cube/gl/renderer.hpp>

# include "window.hpp"

namespace cube { namespace system { namespace window {

	struct Window::Impl
		: private boost::noncopyable
	{
		::SDL_Surface*          screen;
		::cube::gl::renderer::Renderer*   renderer;
# define DECLARE_SIGNAL(name) \
		Window::on_ ## name ## _t    on_##name

		DECLARE_SIGNAL(expose);
		DECLARE_SIGNAL(idle);
		DECLARE_SIGNAL(quit);

# undef DECLARE_SIGNAL

		Impl()
			: screen(nullptr)
			, renderer(nullptr)
		{}
	};

	Window::Window(std::string const& title, uint32_t width, uint32_t height)
		: _impl(new Impl)
		, _title(title)
		, _width(width)
		, _height(height)
	{
		if (::SDL_Init(SDL_INIT_VIDEO))
			throw std::runtime_error(SDL_GetError());
		::SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		::SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
		if (!(_impl->screen = SDL_SetVideoMode(width, height, 0, SDL_RESIZABLE | SDL_OPENGL)))
		{
			::SDL_Quit();
			throw std::runtime_error(SDL_GetError());
		}
		::SDL_WM_SetCaption(title.c_str(), 0);
		//SDL_EnableKeyRepeat(130, 35);
		::SDL_EnableUNICODE(SDL_ENABLE);

		_impl->renderer = ::cube::gl::renderer::create_renderer().release();

		//this->_renderer = new Renderers::GLRenderer();
		//this->_renderer->Initialise();
	}

	Window::~Window()
	{
		std::cout << "~Window(" << _title << ")\n";
		if (_impl->renderer)
			_impl->renderer->shutdown();
		::SDL_Quit();
		delete this->_impl;
	}

#define MAKE_CONNECTOR(evt)                                                    \
	boost::signals::connection                                                 \
	Window::connect_##evt(on_##evt##_t::slot_function_type subscribe_cb)       \
	{                                                                          \
		return this->_impl->on_##evt.connect(subscribe_cb);                    \
	}                                                                          \

	MAKE_CONNECTOR(expose)
	MAKE_CONNECTOR(idle)
	MAKE_CONNECTOR(quit)

#undef MAKE_CONNECTOR

	uint32_t Window::poll()
	{
		return this->poll(100);
	}

	uint32_t Window::poll(uint32_t max)
	{
		uint32_t count = 0;

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_VIDEOEXPOSE:
				_impl->on_expose(_width, _height);
				break;
			case SDL_QUIT:
				_impl->on_quit();
			}
			if (++count >= max)
				break;
		}
		if (count == 0)
			_impl->on_idle();
		return count;
	}

	::cube::gl::renderer::Renderer& Window::renderer()
	{
		return *_impl->renderer;
	}


}}} // !cube::system::window

#endif
