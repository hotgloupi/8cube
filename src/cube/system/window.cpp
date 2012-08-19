#include <wrappers/sdl.hpp>

#include <etc/log.hpp>

#include <cube/gl/renderer.hpp>

#include "Exception.hpp"
#include "inputs.hpp"
#include "window.hpp"

namespace cube { namespace system { namespace window {

	struct Window::Impl
		: private boost::noncopyable
	{
	public:
		typedef std::unique_ptr<gl::renderer::Renderer> RendererPtr;

	public:
		RendererPtr     renderer;
		inputs::Inputs  inputs;
	private:
		::SDL_Surface*  _screen;
		int             _flags;

	public:
		Impl(std::string const& title,
		     uint32_t const width,
			 uint32_t const height,
		     gl::renderer::RendererType::Name const name)
			: renderer{}
			, inputs{}
			, _screen{nullptr}
			, _flags{SDL_RESIZABLE}
		{
			::SDL_WM_SetCaption(title.c_str(), 0);

			//SDL_EnableKeyRepeat(130, 35);
			::SDL_EnableUNICODE(SDL_ENABLE);

			switch (name)
			{
			case gl::renderer::RendererType::OpenGL:
				::SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
				::SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
				_flags |= SDL_OPENGL;
			break;
			default:
				throw "NotImplemented";
			}
			this->resize(width, height);
			gl::viewport::Viewport vp{
				.0f, .0f,
				static_cast<float>(width),
				static_cast<float>(height),
			};
			// SDL_SetVideoMode has been called before (with resize()), we can
			// create the renderer safely.
			this->renderer = std::move(gl::renderer::create_renderer(vp, name));
		}

	public:
		void resize(uint32_t const width, uint32_t const height)
		{
			_screen = SDL_SetVideoMode(width, height, 0, _flags);
			if (_screen == nullptr)
				throw Exception(SDL_GetError());
		}
	};

	Window::Window(std::string const& title,
	               uint32_t const width,
	               uint32_t const height)
		: _impl(nullptr)
		, _title(title)
		, _width(width)
		, _height(height)
	{
		if (::SDL_Init(SDL_INIT_VIDEO))
			throw Exception(SDL_GetError());
		_impl = new Impl{
			title,
			width,
			height,
			gl::renderer::RendererType::OpenGL,
		};
	}

	Window::~Window()
	{
		std::cout << "~Window(" << _title << ")\n";
		if (_impl->renderer)
			_impl->renderer->shutdown();
		::SDL_Quit();
		delete _impl;
		_impl = nullptr;
	}

	uint32_t Window::poll()
	{
		return this->poll(100);
	}

	uint32_t Window::poll(uint32_t max)
	{
		uint32_t count = 0;
		bool has_expose = false;
		bool has_resize = false;

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_VIDEORESIZE:
				_width = e.resize.w;
				_height = e.resize.h;
				has_resize = true;
				break;
			case SDL_VIDEOEXPOSE:
				has_expose = true;
				break;
			case SDL_QUIT:
				_impl->inputs.on_quit()();
				break;
			}
			if (++count >= max)
				break;
		}

		if (has_resize)
		{
			_impl->inputs.on_resize()(_width, _height);
			_impl->resize(_width, _height);
		}
		if (has_expose)
			_impl->inputs.on_expose()(_width, _height);

		if (count == 0)
			_impl->inputs.on_idle()();
		return count;
	}

	::cube::gl::renderer::Renderer& Window::renderer()
	{
		return *_impl->renderer;
	}

	::cube::system::inputs::Inputs& Window::inputs()
	{
		return _impl->inputs;
	}

}}} // !cube::system::window
