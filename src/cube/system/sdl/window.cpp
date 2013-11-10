#include "window.hpp"

#include "../Exception.hpp"
#include "../inputs.hpp"

#include <wrappers/sdl.hpp>

#include <etc/log.hpp>
#include <etc/platform.hpp>

#include <cube/gl/renderer.hpp>

#include <unordered_map>

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

		inputs::KeySym to_keysym(int sym);

	}

	class SDLRendererContext
		: public cube::system::window::RendererContext
	{
		ETC_LOG_COMPONENT("cube.system.sdl.window.RendererContext");
	public:
		SDL_version   linked;
		SDL_version   compiled;
		SDL_Window*   window;
		SDL_GLContext gl_context;
		SDL_SysWMinfo wm;
		static etc::size_type counter;

	public:
		SDLRendererContext(etc::size_type const width,
		                   etc::size_type const height,
		                   Window::Flags const flags,
		                   gl::renderer::Name const name)
		{
			ETC_TRACE.debug(this, "Creating an SDL renderer context");
			SDL_GetVersion(&linked);
			SDL_VERSION(&compiled);

			if (++counter == 1)
			{
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
			}
			int sdl_flags = 0;
			if (name == gl::renderer::Name::OpenGL)
				sdl_flags |= SDL_WINDOW_OPENGL;
			if (flags & Window::Flags::hidden)
				sdl_flags |= SDL_WINDOW_HIDDEN;

			this->window = SDL_CreateWindow(
				"Default name",
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED,
				width,
				width,
				sdl_flags
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

		~SDLRendererContext()
		{
			SDL_DestroyWindow(this->window);
			this->window = nullptr;

			if (--counter == 0)
			{
				ETC_LOG("Last renderer context, quitting SDL");
				::SDL_Quit();
			}
		}
	};

	etc::size_type SDLRendererContext::counter = 0;

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
		ETC_LOG_SUB_COMPONENT("Poll");
		uint32_t count = 0;
		bool has_expose = false;
		bool has_resize = false;

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				ETC_LOG.debug("Got quit event");
			}
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
				ETC_LOG.debug(
					"Got key event",
					e.key.keysym.mod,"=", MOD(e),
					e.key.keysym.sym, "=", to_keysym(e.key.keysym.sym)
				);
				if (e.type == SDL_KEYDOWN)
					this->inputs().on_keydown()(MOD(e), to_keysym(e.key.keysym.sym), 0);
				else
					this->inputs().on_keyup()(MOD(e), to_keysym(e.key.keysym.sym), 0);
			}
			else if (e.type == SDL_MOUSEMOTION)
			{
				ETC_LOG.debug("Mouse motion event");
				this->inputs().on_mousemove()(e.motion.xrel, e.motion.yrel);
			}
			else if (e.type == SDL_FINGERMOTION)
			{
				ETC_LOG.debug("Finger motion event");
				this->inputs().on_mousemove()(e.motion.xrel, e.motion.yrel);
			}
			else
				ETC_LOG.debug("Ignore event", e.type, SDL_MOUSEMOTION);
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
	Window::create_renderer_context(etc::size_type const width,
	                                etc::size_type const height,
	                                Flags const flags,
	                                gl::renderer::Name const name)
	{
		return RendererContextPtr{
			new SDLRendererContext{width, height, flags, name}
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

	namespace {

		inputs::KeySym to_keysym(int sym)
		{
			static
			std::unordered_map<SDL_Keycode, inputs::KeySym> translation_table{
				{SDLK_UNKNOWN, inputs::KeySym::unknown},
				{SDLK_RETURN, inputs::KeySym::return_},
				{SDLK_ESCAPE, inputs::KeySym::escape},
				{SDLK_BACKSPACE, inputs::KeySym::backspace},
				{SDLK_TAB, inputs::KeySym::tab},
				{SDLK_SPACE, inputs::KeySym::space},
				{SDLK_EXCLAIM, inputs::KeySym::exclaim},
				{SDLK_QUOTEDBL, inputs::KeySym::quotedbl},
				{SDLK_HASH, inputs::KeySym::hash},
				{SDLK_PERCENT, inputs::KeySym::percent},
				{SDLK_DOLLAR, inputs::KeySym::dollar},
				{SDLK_AMPERSAND, inputs::KeySym::ampersand},
				{SDLK_QUOTE, inputs::KeySym::quote},
				{SDLK_LEFTPAREN, inputs::KeySym::leftparen},
				{SDLK_RIGHTPAREN, inputs::KeySym::rightparen},
				{SDLK_ASTERISK, inputs::KeySym::asterisk},
				{SDLK_PLUS, inputs::KeySym::plus},
				{SDLK_COMMA, inputs::KeySym::comma},
				{SDLK_MINUS, inputs::KeySym::minus},
				{SDLK_PERIOD, inputs::KeySym::period},
				{SDLK_SLASH, inputs::KeySym::slash},
				{SDLK_0, inputs::KeySym::k0},
				{SDLK_1, inputs::KeySym::k1},
				{SDLK_2, inputs::KeySym::k2},
				{SDLK_3, inputs::KeySym::k3},
				{SDLK_4, inputs::KeySym::k4},
				{SDLK_5, inputs::KeySym::k5},
				{SDLK_6, inputs::KeySym::k6},
				{SDLK_7, inputs::KeySym::k7},
				{SDLK_8, inputs::KeySym::k8},
				{SDLK_9, inputs::KeySym::k9},
				{SDLK_COLON, inputs::KeySym::colon},
				{SDLK_SEMICOLON, inputs::KeySym::semicolon},
				{SDLK_LESS, inputs::KeySym::less},
				{SDLK_EQUALS, inputs::KeySym::equals},
				{SDLK_GREATER, inputs::KeySym::greater},
				{SDLK_QUESTION, inputs::KeySym::question},
				{SDLK_AT, inputs::KeySym::at},
				{SDLK_LEFTBRACKET, inputs::KeySym::leftbracket},
				{SDLK_BACKSLASH, inputs::KeySym::backslash},
				{SDLK_RIGHTBRACKET, inputs::KeySym::rightbracket},
				{SDLK_CARET, inputs::KeySym::caret},
				{SDLK_UNDERSCORE, inputs::KeySym::underscore},
				{SDLK_BACKQUOTE, inputs::KeySym::backquote},
				{SDLK_a, inputs::KeySym::a},
				{SDLK_b, inputs::KeySym::b},
				{SDLK_c, inputs::KeySym::c},
				{SDLK_d, inputs::KeySym::d},
				{SDLK_e, inputs::KeySym::e},
				{SDLK_f, inputs::KeySym::f},
				{SDLK_g, inputs::KeySym::g},
				{SDLK_h, inputs::KeySym::h},
				{SDLK_i, inputs::KeySym::i},
				{SDLK_j, inputs::KeySym::j},
				{SDLK_k, inputs::KeySym::k},
				{SDLK_l, inputs::KeySym::l},
				{SDLK_m, inputs::KeySym::m},
				{SDLK_n, inputs::KeySym::n},
				{SDLK_o, inputs::KeySym::o},
				{SDLK_p, inputs::KeySym::p},
				{SDLK_q, inputs::KeySym::q},
				{SDLK_r, inputs::KeySym::r},
				{SDLK_s, inputs::KeySym::s},
				{SDLK_t, inputs::KeySym::t},
				{SDLK_u, inputs::KeySym::u},
				{SDLK_v, inputs::KeySym::v},
				{SDLK_w, inputs::KeySym::w},
				{SDLK_x, inputs::KeySym::x},
				{SDLK_y, inputs::KeySym::y},
				{SDLK_z, inputs::KeySym::z},
				{SDLK_CAPSLOCK, inputs::KeySym::capslock},
				{SDLK_F1, inputs::KeySym::f1},
				{SDLK_F2, inputs::KeySym::f2},
				{SDLK_F3, inputs::KeySym::f3},
				{SDLK_F4, inputs::KeySym::f4},
				{SDLK_F5, inputs::KeySym::f5},
				{SDLK_F6, inputs::KeySym::f6},
				{SDLK_F7, inputs::KeySym::f7},
				{SDLK_F8, inputs::KeySym::f8},
				{SDLK_F9, inputs::KeySym::f9},
				{SDLK_F10, inputs::KeySym::f10},
				{SDLK_F11, inputs::KeySym::f11},
				{SDLK_F12, inputs::KeySym::f12},
				{SDLK_PAUSE, inputs::KeySym::pause},
				{SDLK_INSERT, inputs::KeySym::insert},
				{SDLK_HOME, inputs::KeySym::home},
				{SDLK_PAGEUP, inputs::KeySym::pageup},
				{SDLK_DELETE, inputs::KeySym::delete_},
				{SDLK_END, inputs::KeySym::end},
				{SDLK_PAGEDOWN, inputs::KeySym::pagedown},
				{SDLK_RIGHT, inputs::KeySym::right},
				{SDLK_LEFT, inputs::KeySym::left},
				{SDLK_DOWN, inputs::KeySym::down},
				{SDLK_UP, inputs::KeySym::up},
				{SDLK_KP_DIVIDE, inputs::KeySym::kp_divide},
				{SDLK_KP_MULTIPLY, inputs::KeySym::kp_multiply},
				{SDLK_KP_MINUS, inputs::KeySym::kp_minus},
				{SDLK_KP_PLUS, inputs::KeySym::kp_plus},
				{SDLK_KP_ENTER, inputs::KeySym::kp_enter},
				{SDLK_KP_1, inputs::KeySym::kp1},
				{SDLK_KP_2, inputs::KeySym::kp2},
				{SDLK_KP_3, inputs::KeySym::kp3},
				{SDLK_KP_4, inputs::KeySym::kp4},
				{SDLK_KP_5, inputs::KeySym::kp5},
				{SDLK_KP_6, inputs::KeySym::kp6},
				{SDLK_KP_7, inputs::KeySym::kp7},
				{SDLK_KP_8, inputs::KeySym::kp8},
				{SDLK_KP_9, inputs::KeySym::kp9},
				{SDLK_KP_0, inputs::KeySym::kp0},
				{SDLK_KP_PERIOD, inputs::KeySym::kp_period}
			};
			return translation_table[sym];
		}
	}

}}}} // !cube::system::sdl::window
