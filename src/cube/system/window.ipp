#ifndef  CUBE_SYSTEM_WINDOW_IPP
# define CUBE_SYSTEM_WINDOW_IPP

# include <cube/gl/Renderer.hpp>

# include "Window.hpp"

# include <SDL/SDL.h>

namespace cube { namespace system {

	struct Window::Impl : private boost::noncopyable
	{
        ::SDL_Surface*          screen;
        ::cube::gl::Renderer*   renderer;
		Window::on_expose_t     on_expose;
		Window::on_idle_t       on_idle;

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

		_impl->renderer = ::cube::gl::create_renderer().release();

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

#undef MAKE_CONNECTOR

	uint32_t Window::poll()
	{
		return this->poll(100);
	}

	uint32_t Window::poll(uint32_t max)
	{
		std::cout << "BIET\n";
		uint32_t count = 0;

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_VIDEOEXPOSE)
				_impl->on_expose(_width, _height);

			if (++count >= max)
				break;
		}
		if (count == 0)
			_impl->on_idle();
		return count;
	}

	cube::gl::Renderer& Window::renderer()
	{
		return *_impl->renderer;
	}


}} // !cube::system

#endif
