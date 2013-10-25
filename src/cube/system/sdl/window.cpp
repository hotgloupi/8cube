#include "window.hpp"

#include "../Exception.hpp"
#include "../inputs.hpp"

#include <wrappers/sdl.hpp>

#include <etc/log.hpp>
#include <etc/platform.hpp>

#include <cube/gl/renderer.hpp>


namespace cube { namespace system { namespace sdl { namespace window {

	ETC_LOG_COMPONENT("cube.system.sdl.window.Window");

	struct Window::Impl
		: private boost::noncopyable
	{
		::SDL_Window*           screen;
		//::SDL_VideoInfo const*  video_info;
		int                     flags;
		int                     screen_bpp;

		Impl()
			: screen{nullptr}
			//, video_info{nullptr}
			, flags{0}
			, screen_bpp{0}
		{}

		void resize(etc::size_type const width,
		            etc::size_type const height)
		{
			ETC_LOG.debug(
				"Setting SDL video mode with",
				width, height, this->screen_bpp, this->flags
			);

			/*
			if (this->video_info == nullptr)
				this->video_info = SDL_GetVideoInfo();
			if (this->video_info == nullptr)
				throw Exception{etc::to_string(
					"Coudn't get video information!", SDL_GetError()
				)};

			if (this->screen == nullptr)
			{
				if (this->video_info->hw_available)
					this->flags |= SDL_HWSURFACE;
				else
					this->flags |= SDL_SWSURFACE;

				if (this->video_info->blit_hw)
					this->flags |= SDL_HWACCEL;
			}
			else
			{
				//::SDL_FreeSurface(this->screen);
				//this->screen = nullptr;
			}

			this->screen = SDL_SetVideoMode(
				width, height, 0, this->flags
			);

			SDL_GL_SwapBuffers();

			*/

		}
	};

	Window::Window(std::string const& title,
	               etc::size_type const width,
	               etc::size_type const height,
	               gl::renderer::Name const renderer_name)
		: cube::system::window::Window{title, width, height, renderer_name}
		, _sdl_impl{new Impl{}}
	{
		SDL_version linked;
#if 1
		SDL_GetVersion(&linked);
#else // old SDL 1.2
		SDL_version const* linked_ptr = SDL_Linked_Version();
		std::memcpy(&linked, linked_ptr, sizeof(linked));
#endif

		SDL_version compiled;
		SDL_VERSION(&compiled);
		ETC_LOG(
			"Compiled SDL version ", etc::iomanip::nosep(),
			(int)compiled.major, '.', (int)compiled.minor, '-', (int)compiled.patch
		);
		ETC_LOG(
			"Linked SDL version ", etc::iomanip::nosep(),
			(int)linked.major, '.', (int)linked.minor, '-', (int)linked.patch
		);
		if (::SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
			throw Exception(std::string{"SDL_Init(): "} + SDL_GetError());

		_sdl_impl->screen = SDL_CreateWindow(
			title.c_str(),
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			width,
			height,
			SDL_WINDOW_OPENGL
		);
		if (_sdl_impl->screen == nullptr)
			throw Exception{
				std::string{"SDL_CreateWindow(): "} + SDL_GetError()
			};
		SDL_GL_CreateContext(_sdl_impl->screen);
#ifdef ETC_PLATFORM_MACOSX
		_sdl_impl->screen_bpp = 32;
#else
		_sdl_impl->screen_bpp = 16;
#endif
		//::SDL_WM_SetCaption(title.c_str(), 0);

		//SDL_EnableKeyRepeat(130, 35);
		//::SDL_EnableUNICODE(SDL_ENABLE);
/*
		switch (renderer_name)
		{
		case gl::renderer::Name::OpenGL:
			_sdl_impl->flags |= SDL_OPENGL;
			//_sdl_impl->flags |= SDL_GL_DOUBLEBUFFER;
			::SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			//::SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
			::SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, _sdl_impl->screen_bpp);
			break;
		default:
			throw Exception("Other renderer than OpenGL are not supported");
		}
		_sdl_impl->resize(width, height); */
		gl::viewport::Viewport vp{
			.0f, .0f,
			static_cast<float>(width),
			static_cast<float>(height),
		};

		ETC_LOG.debug("SDL has been initialized, creating the renderer");
		// SDL_SetVideoMode has been called before (with resize()), we can
		// create the renderer safely.
		//this->renderer = std::move(gl::renderer::create_renderer(vp, name));
	}

	void Window::swap_buffers()
	{
        ETC_LOG.debug("Swapping buffers");
		SDL_GL_SwapWindow(_sdl_impl->screen);
	}

	//Window::Window(std::string const& title,
	//               uint32_t const width,
	//               uint32_t const height)
	//	: _sdl_impl(nullptr)
	//	, _title(title)
	//	, _width(width)
	//	, _height(height)
	//{
	//	ETC_TRACE.debug("Creating a window", title, "(", width, 'x', height, ')');
	//	if (::SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	//		throw Exception(std::string{"SDL_Init(): "} + SDL_GetError());
	//	_sdl_impl = new Impl{
	//		title,
	//		width,
	//		height,
	//		gl::renderer::RendererType::OpenGL,
	//	};
	//}

	Window::~Window()
	{
		::SDL_Quit();
	}

	etc::size_type
	Window::poll(etc::size_type const max)
	{
		ETC_TRACE.debug("Polling events");
		uint32_t count = 0;
		bool has_expose = false;
		bool has_resize = false;

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_WINDOWEVENT)
			{
				switch (e.window.event)
				{
				case SDL_WINDOWEVENT_RESIZED:
					_width = e.window.data1;
					_height = e.window.data2;
					has_resize = true;
					break;
				case SDL_WINDOWEVENT_EXPOSED:
					has_expose = true;
					break;
				case SDL_WINDOWEVENT_CLOSE:
					this->inputs().on_quit()();
					break;
				default:
					ETC_LOG.debug("Ignore window event", e.window.event);
				}
			}
			else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
			{
				if (e.key.keysym.sym == SDLK_ESCAPE)
				{
					this->inputs().on_quit()();
					break;
				}
#define MOD(e) static_cast<inputs::KeyMod>(static_cast<int>(e.key.keysym.mod))
#define SYM(e) static_cast<inputs::KeySym>(static_cast<int>(e.key.keysym.sym))
/*
#define CHR(e) (                                                              \
		((e.key.keysym.unicode & 0xFF80) == 0)                                \
		? e.key.keysym.unicode & 0x7F                                         \
		: e.key.keysym.unicode                                                \
	)                                                                         \
*/
#define CHR(e) 0
				if (e.type == SDL_KEYDOWN)
					this->inputs().on_keydown()(MOD(e), SYM(e), CHR(e));
				else
					this->inputs().on_keyup()(MOD(e), SYM(e), CHR(e));
			}
			else if (e.type == SDL_MOUSEMOTION)
			{
				this->inputs().on_mousemove()(e.motion.xrel, e.motion.yrel);
			}
			if (++count >= max)
				break;
		}

		if (has_resize)
		{
			ETC_TRACE.debug("Got resize event", _width, _height);
			_sdl_impl->resize(_width, _height);
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


	void Window::confine_mouse(bool mode)
	{
#if 0
		SDL_GrabMode input_mode = mode ? SDL_GRAB_ON : SDL_GRAB_OFF;
		::SDL_WM_GrabInput(input_mode);
		if (input_mode != ::SDL_WM_GrabInput(SDL_GRAB_QUERY))
			throw Exception{etc::to_string(
				"Cannot ", mode ? "enable" : "disable", "mouse grabbing"
			)};
		SDL_ShowCursor(!mode ? SDL_ENABLE : SDL_DISABLE);

		if (mode)
			SDL_WarpMouse(_width / 2, _height / 2);
#endif

		SDL_SetRelativeMouseMode(mode ? SDL_TRUE : SDL_FALSE);
		SDL_Event e;
		SDL_PumpEvents();

		while (
#ifdef SDL_EVENTMASK
			SDL_PeepEvents(&e, 1, SDL_GETEVENT, SDL_EVENTMASK(SDL_MOUSEMOTION)) > 0
#else
			SDL_PeepEvents(&e, 1, SDL_GETEVENT, SDL_MOUSEMOTION, SDL_MOUSEMOTION) > 0
#endif
		) { /* Dry the queue of mouse motion events */ }
	}

	// Check that SDL implem won't change
	static_assert((int) inputs::KeyMod::none    == KMOD_NONE,
	              "inputs::KeyMod::none    == KMOD_NONE");
	static_assert((int) inputs::KeyMod::lshift  == KMOD_LSHIFT,
	              "inputs::KeyMod::lshift  == KMOD_LSHIFT");
	static_assert((int) inputs::KeyMod::rshift  == KMOD_RSHIFT,
	              "inputs::KeyMod::rshift  == KMOD_RSHIFT");
	static_assert((int) inputs::KeyMod::lctrl   == KMOD_LCTRL,
	              "inputs::KeyMod::lctrl   == KMOD_LCTRL");
	static_assert((int) inputs::KeyMod::rctrl   == KMOD_RCTRL,
	              "inputs::KeyMod::rctrl   == KMOD_RCTRL");
	static_assert((int) inputs::KeyMod::lalt    == KMOD_LALT,
	              "inputs::KeyMod::lalt    == KMOD_LALT");
	static_assert((int) inputs::KeyMod::ralt    == KMOD_RALT,
	              "inputs::KeyMod::ralt    == KMOD_RALT");
	static_assert((int) inputs::KeyMod::lmeta   == KMOD_LGUI,
	              "inputs::KeyMod::lmeta   == KMOD_LGUI");
	static_assert((int) inputs::KeyMod::rmeta   == KMOD_RGUI,
	              "inputs::KeyMod::rmeta   == KMOD_RGUI");
	static_assert((int) inputs::KeyMod::num     == KMOD_NUM,
	              "inputs::KeyMod::num     == KMOD_NUM");
	static_assert((int) inputs::KeyMod::caps    == KMOD_CAPS,
	              "inputs::KeyMod::caps    == KMOD_CAPS");
	static_assert((int) inputs::KeyMod::mode    == KMOD_MODE,
	              "inputs::KeyMod::mode    == KMOD_MODE");
	static_assert((int) inputs::KeyMod::ctrl    == KMOD_CTRL,
	              "inputs::KeyMod::ctrl    == KMOD_CTRL");
	static_assert((int) inputs::KeyMod::shift   == KMOD_SHIFT,
	              "inputs::KeyMod::shift   == KMOD_SHIFT");
	static_assert((int) inputs::KeyMod::alt     == KMOD_ALT,
	              "inputs::KeyMod::alt     == KMOD_ALT");
	static_assert((int) inputs::KeyMod::meta    == KMOD_GUI,
	              "inputs::KeyMod::meta    == KMOD_GUI");

}}}} // !cube::system::sdl::window
