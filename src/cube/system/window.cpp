#include <wrappers/sdl.hpp>

#include <etc/log.hpp>

#include <cube/gl/renderer.hpp>

#include "Exception.hpp"
#include "inputs.hpp"
#include "window.hpp"

namespace cube { namespace system { namespace window {

	ETC_LOG_COMPONENT("cube.system.window.Window");

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
		ETC_TRACE.debug("Polling events");
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
			case SDL_KEYDOWN:
				printf("KEYYYY\n");
				if (e.key.keysym.sym == SDLK_ESCAPE)
				{
					_impl->inputs.on_quit()();
					break;
				}
				inputs::KeyMod mod = static_cast<inputs::KeyMod>(
					static_cast<int>(e.key.keysym.mod)
				);
				char ch;
				if ((e.key.keysym.unicode & 0xFF80) == 0)
				{
					ch = e.key.keysym.unicode & 0x7F;
					if (ch != 0)
						_impl->inputs.on_keydown()(mod, ch);
				}
				else
					_impl->inputs.on_keydown()(mod, e.key.keysym.unicode);
				printf("Key Sent!\n");
				break;
			}
			if (++count >= max)
				break;
		}

		if (has_resize)
		{
			ETC_TRACE.debug("Got resize event", _width, _height);
			_impl->inputs.on_resize()(_width, _height);
			_impl->resize(_width, _height);
		}
		if (has_expose)
		{
			ETC_TRACE.debug("Got expose event");
			_impl->inputs.on_expose()(_width, _height);
		}

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

	// Check that SDL implem won't change
	static_assert((int) inputs::KeyMod::none    == KMOD_NONE, "inputs::KeyMod::none    == KMOD_NONE");
	static_assert((int) inputs::KeyMod::lshift  == KMOD_LSHIFT, "inputs::KeyMod::lshift  == KMOD_LSHIFT");
	static_assert((int) inputs::KeyMod::rshift  == KMOD_RSHIFT, "inputs::KeyMod::rshift  == KMOD_RSHIFT");
	static_assert((int) inputs::KeyMod::lctrl   == KMOD_LCTRL, "inputs::KeyMod::lctrl   == KMOD_LCTRL");
	static_assert((int) inputs::KeyMod::rctrl   == KMOD_RCTRL, "inputs::KeyMod::rctrl   == KMOD_RCTRL");
	static_assert((int) inputs::KeyMod::lalt    == KMOD_LALT, "inputs::KeyMod::lalt    == KMOD_LALT");
	static_assert((int) inputs::KeyMod::ralt    == KMOD_RALT, "inputs::KeyMod::ralt    == KMOD_RALT");
	static_assert((int) inputs::KeyMod::lmeta   == KMOD_LMETA, "inputs::KeyMod::lmeta   == KMOD_LMETA");
	static_assert((int) inputs::KeyMod::rmeta   == KMOD_RMETA, "inputs::KeyMod::rmeta   == KMOD_RMETA");
	static_assert((int) inputs::KeyMod::num     == KMOD_NUM, "inputs::KeyMod::num     == KMOD_NUM");
	static_assert((int) inputs::KeyMod::caps    == KMOD_CAPS, "inputs::KeyMod::caps    == KMOD_CAPS");
	static_assert((int) inputs::KeyMod::mode    == KMOD_MODE, "inputs::KeyMod::mode    == KMOD_MODE");
	static_assert((int) inputs::KeyMod::ctrl    == KMOD_CTRL, "inputs::KeyMod::ctrl    == KMOD_CTRL");
	static_assert((int) inputs::KeyMod::shift   == KMOD_SHIFT, "inputs::KeyMod::shift   == KMOD_SHIFT");
	static_assert((int) inputs::KeyMod::alt     == KMOD_ALT, "inputs::KeyMod::alt     == KMOD_ALT");
	static_assert((int) inputs::KeyMod::meta    == KMOD_META, "inputs::KeyMod::meta    == KMOD_META");

}}} // !cube::system::window
