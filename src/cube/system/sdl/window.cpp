#include "window.hpp"

#include "../Exception.hpp"
#include "../inputs.hpp"

#include <wrappers/sdl.hpp>

#include <etc/log.hpp>
#include <etc/platform.hpp>

#include <cube/gl/renderer.hpp>


namespace cube { namespace system { namespace sdl { namespace window {

	ETC_LOG_COMPONENT("cube.system.sdl.window.Window");

	namespace {

		class SDLException : public Exception
		{
		public:
			SDLException(std::string const& funcname)
				: Exception{"SDL_" + funcname + ": " + SDL_GetError()}
			{}
		};

	}

	class SDLRendererContext
		: public cube::system::window::RendererContext
	{
	public:
		SDL_version   linked;
		SDL_version   compiled;
		SDL_Window*   window;
		SDL_GLContext gl_context;
		SDL_SysWMinfo wm;

	public:
		SDLRendererContext()
		{
			SDL_GetVersion(&linked);
			SDL_VERSION(&compiled);

			ETC_LOG(
				"Compiled SDL version ", etc::iomanip::nosep(),
				(int)this->compiled.major, '.', (int)this->compiled.minor, '-',
				(int)this->compiled.patch
			);
			ETC_LOG(
				"Linked SDL version ", etc::iomanip::nosep(),
				(int)this->linked.major, '.', (int)this->linked.minor, '-',
				(int)this->linked.patch
			);
			if (::SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
				throw SDLException("Init");
			this->window = SDL_CreateWindow(
				"Default name",
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED,
				640,
				480,
				SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN
			);
			if (this->window == nullptr)
				throw SDLException{"CreateWindow"};

			this->gl_context = SDL_GL_CreateContext(this->window);
			if (this->gl_context == nullptr)
				throw SDLException{"GL_CreateContext"};

			SDL_VERSION(&this->wm.version);
			if (SDL_GetWindowWMInfo(this->window, &this->wm) == SDL_FALSE)
				throw SDLException{"GetWindowWMInfo"};
/*
			SDL_WindowShapeMode shape_mode;
			shape_mode.mode = ShapeModeDefault;
			//shape_mode.parameters.colorKey = {
			//	255, 0, 0, 0
			//};
			SDL_Surface* shape = SDL_CreateRGBSurface(
				0,   // flags,
				640, // width,
				480, // height,
				32,   //  depth,
				0, 0, 0, 0 //Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
			);


			if (SDL_SetWindowShape(_context().window, SDL_GetWindowSurface(_context().window), &shape_mode))
				throw SDLException{"SetWindowShape"};
*/
		}
	};

	Window::Window(ImplPtr&& impl)
		: Super{std::move(impl)}
	{
		SDL_SetWindowTitle(_context().window, this->title().c_str());
		SDL_SetWindowSize(_context().window, this->width(), this->height());

		if (this->flags() & Flags::borderless)
			SDL_SetWindowBordered(_context().window, SDL_FALSE);
		if (not (this->flags() & Flags::hidden))
			SDL_ShowWindow(_context().window);
	}

	void Window::swap_buffers()
	{
        ETC_LOG.debug("Swapping buffers");
		SDL_GL_SwapWindow(_context().window);
	}

	Window::~Window()
	{
		::SDL_Quit();
	}

	SDLRendererContext& Window::_context()
	{
#ifdef CUBE_DEBUG
		return dynamic_cast<SDLRendererContext&>(this->renderer_context());
#else
		return static_cast<SDLRendererContext&>(this->renderer_context());
#endif
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
					this->width(e.window.data1);
					this->height(e.window.data2);
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
			ETC_TRACE.debug("Got resize event", this->width(), this->height());
//_sdl_impl->resize(_impl->width, _impl->height);
			this->inputs().on_resize()(this->width(), this->height());
		}
		if (has_expose)
		{
			ETC_TRACE.debug("Got expose event");
			this->inputs().on_expose()(this->width(), this->height());
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


	Window::RendererContextPtr
	Window::create_renderer_context(gl::renderer::Name const name)
	{
		return RendererContextPtr{
			new SDLRendererContext{}
		};
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
