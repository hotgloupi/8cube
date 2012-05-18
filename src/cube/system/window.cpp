#include <iostream>

#include <stdexcept>

#include "window.hpp"

#include <SDL/SDL.h>

namespace cube { namespace system {

	struct Window::Impl
	{
        ::SDL_Surface*          screen;
		Window::on_expose_t     on_expose;
		Window::on_idle_t       on_idle;
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
        if (!(this->_impl->screen = SDL_SetVideoMode(width, height, 0, SDL_RESIZABLE | SDL_OPENGL)))
        {
            ::SDL_Quit();
            throw std::runtime_error(SDL_GetError());
        }
        ::SDL_WM_SetCaption(title.c_str(), 0);
        //SDL_EnableKeyRepeat(130, 35);
        ::SDL_EnableUNICODE(SDL_ENABLE);
        //this->_renderer = new Renderers::GLRenderer();
        //this->_renderer->Initialise();
	}

	Window::~Window()
	{
		std::cout << "~Window(" << _title << ")\n";
		::SDL_Quit();
		delete this->_impl;
	}

	boost::signals::connection Window::connect_expose(on_expose_t::slot_function_type subscribe_cb)
	{
		this->_impl->on_expose.connect(subscribe_cb);
	}

	boost::signals::connection Window::connect_idle(on_idle_t::slot_function_type subscribe_cb)
	{
		this->_impl->on_idle.connect(subscribe_cb);
	}

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
			if (e.type == SDL_VIDEOEXPOSE)
				_impl->on_expose(_width, _height);

			if (++count >= max)
				break;
		}
		return count;
	}

}}

#include <wrappers/boost/python.hpp>
namespace py = boost::python;

BOOST_PYTHON_MODULE(window)
{
	using namespace cube::system;

	py::class_<Window>(
			"Window",
			"Main window",
			py::init<std::string const&, uint32_t, uint32_t>(
				py::args("title", "width", "height"),
				"Window ctor"
			)
		)
		.def("connect_expose", &Window::connect_expose)
		.def("connect_idle", &Window::connect_idle)
		.def("poll", static_cast<uint32_t (Window::*)()>(&Window::poll))
		.def("poll", static_cast<uint32_t (Window::*)(uint32_t)>(&Window::poll))
	;

	py::class_<boost::signals::connection>(
			"Slot",
			"Window's events slot"
		)
	;
}
