#include "window.hpp"

#include <cube/gl/renderer.hpp>
#include <cube/gl/renderer/RenderTarget.hpp>
#include <cube/gl/renderer/Texture.hpp>
#include <cube/gl/renderer/opengl/_opengl.hpp>
#include <cube/system/Exception.hpp>
#include <cube/system/inputs.hpp>

#include <wrappers/sdl.hpp>

#include <etc/log.hpp>
#include <etc/platform.hpp>
#include <etc/sys/environ.hpp>
#include <etc/scope_exit.hpp>

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

	class SDLRenderTarget
		: public gl::renderer::RenderTarget
	{
	private:
		etc::size_type _width;
		etc::size_type _height;
		SDL_Texture* _texture;
		SDL_Window* _window;
		SDL_Renderer* _renderer;
		SDL_Surface* _renderer_surface;
		SDL_Texture* _previous;

	public:
		SDLRenderTarget(SDL_Renderer* renderer,
		                SDL_Surface* renderer_surface,
		                SDL_Window* window,
		                etc::size_type const width,
		                etc::size_type const height,
		                uint32_t const sdl_format)
			: _width{width}
			, _height{height}
			, _texture{nullptr}
			, _window{window}
			, _renderer{renderer}
			, _renderer_surface{renderer_surface}
			, _previous{nullptr}
		{
			ETC_TRACE.debug("Creating", *this);
			assert(_renderer != nullptr);
			_texture = SDL_CreateTexture(
				_renderer,
				sdl_format, //SDL_PIXELFORMAT_RGBA8888,
				SDL_TEXTUREACCESS_TARGET,
				width,
				height
			);
			if (_texture == nullptr)
				throw SDLException{"CreateTexture"};
		}

		~SDLRenderTarget()
		{
			ETC_TRACE.debug("Destroying", *this);
			SDL_DestroyTexture(_texture);
		}

		void _bind(gl::renderer::State const&) override
		{
			ETC_TRACE.debug("Binding", *this, "with", _texture);
			_previous = SDL_GetRenderTarget(_renderer);
			if (SDL_SetRenderTarget(_renderer, _texture) != 0)
				throw SDLException{"SetRenderTarget"};
		}

		void _unbind() noexcept override
		{
			ETC_TRACE.debug("Unbinding", *this, "to", _texture);
			if (SDL_SetRenderTarget(_renderer, _previous) != 0)
				ETC_LOG.error(
					"Couldn't set previous render target (SetRenderTarget:" +
					std::string(SDL_GetError()) + ")"
				);
		}

		gl::renderer::TexturePtr texture() const override
		{
			struct Texture
				: public gl::renderer::Texture
			{
				SDL_Texture* _texture;

				inline
				Texture(SDL_Texture* texture) noexcept
					: _texture{texture}
				{}

				void
				set_data(unsigned int x,
						 unsigned int y,
						 unsigned int width,
						 unsigned int height,
						 gl::renderer::PixelFormat const data_format,
						 gl::renderer::ContentPacking const data_packing,
						 void const* data) override
				{ throw Exception{"Not implemented"}; }

				void
				bind_unit(etc::size_type unit,
				          gl::renderer::ShaderProgramParameter& param)
				{ throw Exception{"Not implemented"}; }

				void _bind() override
				{
					if (SDL_GL_BindTexture(_texture, nullptr, nullptr) != 0)
						throw SDLException("GL_BindTexture");
				}
				void _unbind() noexcept override
				{ SDL_GL_UnbindTexture(_texture); }
			};

			return gl::renderer::TexturePtr{new Texture{_texture}};
		}

		void
		save(std::string const& file) const
		{
			ETC_TRACE.debug("Save", *this, "to", file);
			if (SDL_GL_BindTexture(_texture, nullptr, nullptr) != 0)
				throw SDLException("GL_BindTexture");
			ETC_SCOPE_EXIT{ SDL_GL_UnbindTexture(_texture); };
			int width, height, fmt, red_size, green_size, blue_size, alpha_size;
			glGetTexLevelParameteriv(GL_TEXTURE_RECTANGLE, 0, GL_TEXTURE_WIDTH, &width);
			glGetTexLevelParameteriv(GL_TEXTURE_RECTANGLE, 0, GL_TEXTURE_HEIGHT, &height);
			glGetTexLevelParameteriv(GL_TEXTURE_RECTANGLE, 0, GL_TEXTURE_RED_SIZE, &red_size);
			glGetTexLevelParameteriv(GL_TEXTURE_RECTANGLE, 0, GL_TEXTURE_GREEN_SIZE, &green_size);
			glGetTexLevelParameteriv(GL_TEXTURE_RECTANGLE, 0, GL_TEXTURE_BLUE_SIZE, &blue_size);
			glGetTexLevelParameteriv(GL_TEXTURE_RECTANGLE, 0, GL_TEXTURE_ALPHA_SIZE, &alpha_size);
			glGetTexLevelParameteriv(GL_TEXTURE_RECTANGLE, 0, GL_TEXTURE_INTERNAL_FORMAT, &fmt);
			ETC_LOG.debug("Internal texture size is", width, height);
			ETC_LOG.debug("Internal texture is", gl::renderer::opengl::gl::to_pixel_format(fmt));
			ETC_LOG.debug("Component sizes:", red_size, green_size, blue_size, alpha_size);
			int bpp = 4;
			std::unique_ptr<char[]> pixels{new char[width * height * bpp]};
			memset(pixels.get(), 0, width * height * bpp);
			gl::renderer::opengl::gl::GetTexImage(
				GL_TEXTURE_RECTANGLE,
				0,
				GL_BGRA,
				GL_UNSIGNED_INT_8_8_8_8_REV,
				(void*)pixels.get()
			);
			int pitch = bpp * width;
			std::unique_ptr<char[]> row{new char[pitch]};
			for (int line = 0; 2 * line < height - 1; line++)
			{
				char* down_row = pixels.get() + line * pitch;
				char* up_row = pixels.get() + (height - 1 - line) * pitch;
				memcpy(row.get(), down_row, pitch); // save down
				memcpy(down_row, up_row, pitch); // copy up to down
				memcpy(up_row, row.get(), pitch); // copy down to up
			}
			SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
				pixels.get(),
				width,
				height,
				bpp * 8,
				pitch,
				0, 0, 0, 0
				//0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000
			);
			if (surface == nullptr)
				throw SDLException{"CreateRGBSurfaceFrom"};
			ETC_SCOPE_EXIT{ SDL_FreeSurface(surface); };
			if (SDL_SaveBMP(surface, file.c_str()) != 0)
				throw SDLException("SDL_SaveBMP");
		}
	};

	class SDLRendererContext
		: public cube::system::window::RendererContext
	{
		ETC_LOG_COMPONENT("cube.system.sdl.window.RendererContext");
	public:
		SDL_version           linked;
		SDL_version           compiled;
		SDL_Window*           window;
		SDL_Renderer*         renderer;
		SDL_Surface*          renderer_surface; //only for software renderer
		SDL_RendererInfo      renderer_info;
		SDL_GLContext         gl_context;
		SDL_SysWMinfo         wm;
		static etc::size_type counter;

	public:
		SDLRendererContext(etc::size_type const width,
		                   etc::size_type const height,
		                   Window::Flags const flags,
		                   gl::renderer::Name const name)
			: RendererContext{width, height, flags, name}
		{
			ETC_TRACE.debug("Create", *this);
			SDL_GetVersion(&linked);
			SDL_VERSION(&compiled);

			if (++counter == 1)
			{
				ETC_TRACE("First renderer context, initialize SDL");
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
				SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				width,
				height,
				sdl_flags
			);
			if (this->window == nullptr)
				throw SDLException{"CreateWindow"};

			auto window_guard = etc::scope_exit(
				[&] { SDL_DestroyWindow(window); }
			);
			if (etc::sys::environ::contains("CUBE_SYSTEM_WINDOW_NO_RENDERER"))
			{
				ETC_LOG.warn("Renderer creation was disabled");
				this->renderer = nullptr;
				this->renderer_surface = nullptr;
			}
			else if (etc::sys::environ::contains("CUBE_SYSTEM_WINDOW_SOFTWARE_RENDERER"))
			{
				ETC_LOG.debug("Creating a software renderer");
				this->renderer_surface = SDL_GetWindowSurface(this->window);
				if (this->renderer_surface == nullptr)
					throw SDLException{"GetWindowSurface"};
				this->renderer = SDL_CreateSoftwareRenderer(this->renderer_surface);
				if (this->renderer == nullptr)
					ETC_LOG.warn("Couldn't create a renderer:", SDL_GetError());
			}
			else
			{
				ETC_LOG.debug("Creating window and renderer");
				this->renderer = SDL_CreateRenderer(
					this->window,
					-1,
					SDL_RENDERER_ACCELERATED |
					SDL_RENDERER_PRESENTVSYNC |
					SDL_RENDERER_TARGETTEXTURE
				);
				if (this->renderer == nullptr)
					throw SDLException{"CreateWindowAndRenderer"};
				this->renderer_surface = nullptr;
			}

			if (this->renderer != nullptr)
			{
				ETC_LOG.debug("Retreive renderer infos");
				if (SDL_GetRendererInfo(this->renderer, &this->renderer_info) != 0)
					throw SDLException{"GetRendererInfo"};

				ETC_LOG.debug("SDL renderer name:", this->renderer_info.name);
			}
			else
			{
				memset(&this->renderer_info, 0, sizeof(this->renderer_info));
			}

			ETC_LOG.debug("Create opengl context");
			this->gl_context = SDL_GL_CreateContext(this->window);
			if (this->gl_context == nullptr)
				throw SDLException{"GL_CreateContext"};

			ETC_LOG.debug("Retreive Window handle");
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

			window_guard.dismiss();
		}


		void _size(etc::size_type const width,
		           etc::size_type const height) override
		{
			SDL_SetWindowSize(this->window, width, height);
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

		bool allow_render_target() const noexcept
		{ return this->renderer != nullptr and this->renderer_info.flags & SDL_RENDERER_TARGETTEXTURE; }

		gl::renderer::RenderTargetPtr
		new_render_target(etc::size_type const width,
		                  etc::size_type const height) override
		{
			if (not this->allow_render_target())
				throw Exception{"RenderTarget not allowed by this renderer"};
			return gl::renderer::RenderTargetPtr{
				new SDLRenderTarget{
					this->renderer,
					this->renderer_surface,
					this->window,
					width,
					height,
					this->renderer_info.texture_formats[0]
				}
			};
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

	void
	Window::screenshot(std::string const& file) const
	{
		ETC_TRACE.debug("Save from window's surface to", file);
		SDL_Surface* surface = SDL_GetWindowSurface(_context().window);
		if (surface != nullptr)
		{
			if (SDL_SaveBMP(surface, file.c_str()) != 0)
				throw SDLException("SaveBMP");
			return;
		}

		//if (this->_renderer == nullptr)
		//	throw Exception{"No renderer available"};

		int width, height;
		if (SDL_GetRendererOutputSize(_context().renderer, &width, &height) != 0)
		{
			width = this->width();
			height = this->height();
			ETC_LOG.warn(
				"Couldn't get render output size:", SDL_GetError(),
				"(fallback on render target size:", width, height, ")");
		}


		int bpp = 4;
		std::unique_ptr<char[]> pixels{new char[width * height * bpp]};
		int pitch = bpp * width;
		if (SDL_RenderReadPixels(_context().renderer, nullptr, 0, pixels.get(), pitch) != 0)
			throw SDLException{"RenderReadPixels"};

		surface = SDL_CreateRGBSurfaceFrom(
			pixels.get(),
			width,
			height,
			bpp * 8,
			pitch,
			0,
			0,
			0,
			0
		);
		if (surface == nullptr)
			throw SDLException{"CreateRGBSurfaceFrom"};
		ETC_SCOPE_EXIT{ SDL_FreeSurface(surface); };
		if (SDL_SaveBMP(surface, file.c_str()) != 0)
			throw SDLException("SDL_SaveBMP");
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
					this->size(e.window.data1, e.window.data2);
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

	void Window::show()
	{ SDL_ShowWindow(_context().window); }

	void Window::raise()
	{ SDL_RaiseWindow(_context().window); }

	void Window::hide()
	{ SDL_HideWindow(_context().window); }

	void Window::minimize()
	{ SDL_MinimizeWindow(_context().window); }

	void Window::maximize()
	{ SDL_MaximizeWindow(_context().window); }

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
