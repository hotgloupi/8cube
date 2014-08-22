#include "Window.hpp"
#include "RocketRenderInterface.hpp"

#include <cube/system/window.hpp>
#include <cube/system/inputs.hpp>
#include <cube/exception.hpp>
#include <cube/gl/renderer/Renderer.hpp>
#include <cube/gl/renderer/Painter.hpp>

#include <etc/assert.hpp>
#include <etc/log.hpp>
#include <etc/stack_ptr.hpp>

#include <atomic>

namespace cube { namespace gui { namespace window {

	using cube::exception::Exception;

	ETC_LOG_COMPONENT("cube.gui.Window");


	struct Window::Impl
	{
		typedef Rocket::Core::Context context_t;
		typedef std::unique_ptr<context_t, void(*)(context_t*)> context_ptr_t;

		std::unique_ptr<cube::system::window::Window> root_window;
		std::unique_ptr<RocketRenderInterface> render_interface;
		context_ptr_t rocket_context;
		etc::stack_ptr<gl::vector::vec2i> new_size;
		boost::signals2::connection on_resize_connection;
		boost::signals2::connection on_expose_connection;
		boost::signals2::connection on_keydown_connection;
		boost::signals2::connection on_keyup_connection;
		boost::signals2::connection on_mousedown_connection;
		boost::signals2::connection on_mouseup_connection;
		boost::signals2::connection on_mousemove_connection;

		Impl()
			: rocket_context(nullptr, &_delete_rocket_context)
			, new_size{etc::stack_ptr_no_init}
		{}

		~Impl()
		{
			this->on_resize_connection.disconnect();
			this->on_expose_connection.disconnect();
			this->on_keydown_connection.disconnect();
			this->on_keyup_connection.disconnect();
			this->on_mousedown_connection.disconnect();
			this->on_mouseup_connection.disconnect();
			this->on_mousemove_connection.disconnect();
		}

		void on_resize(int w, int h)
		{ this->new_size.reset(w, h); }

		void on_expose(int w, int h)
		{ this->new_size.reset(w, h); }

		void on_key(bool down,
		            system::inputs::KeyMod mod,
		            system::inputs::KeySym sym,
		            uint16_t)
		{
			ETC_TRACE.debug("Got a key", (down ? "press" : "release"), mod, sym);
			Rocket::Core::Input::KeyIdentifier key_id = _to_rocket_sym(sym);
			int key_mod = _to_rocket_mod(mod);
			if (down)
				this->rocket_context->ProcessKeyDown(key_id, key_mod);
			else
				this->rocket_context->ProcessKeyUp(key_id, key_mod);
		}

		void on_mouse(bool down, uint8_t button, system::inputs::KeyMod mod)
		{
			ETC_TRACE.debug("Got a mouse", (down ? "press" : "release"), +button, mod);
			int key_mod = _to_rocket_mod(mod);
			if (down)
				this->rocket_context->ProcessMouseButtonDown(button, key_mod);
			else
				this->rocket_context->ProcessMouseButtonUp(button, key_mod);
		}

		void on_mousemove(int32_t, int32_t, system::inputs::KeyMod mod)
		{
			auto pos = this->root_window->mouse_position();
			ETC_TRACE.debug("Got a mouse move", pos, mod);
			this->rocket_context->ProcessMouseMove(pos.x, pos.y, _to_rocket_mod(mod));
		}
	private:
		static void _delete_rocket_context(Rocket::Core::Context* ctx)
		{ ctx->RemoveReference(); }

		static
		Rocket::Core::Input::KeyIdentifier
		_to_rocket_sym(system::inputs::KeySym sym);

		static
		int _to_rocket_mod(system::inputs::KeyMod mod);
	};


	Window::Window(int width, int height, std::string const& title)
		: _this{new Impl}
	{
		ETC_TRACE_CTOR(width, height, title);
		_this->root_window = cube::system::window::Window::create(
			title,
			width, height
		);
		static std::atomic<int> count{0};
		std::string context_name = etc::to_string(
		    "window", title, "(", count++, ")"
		);
		_this->render_interface.reset(
			new RocketRenderInterface{_this->root_window->renderer()}
		);

		_this->rocket_context.reset(
			Rocket::Core::CreateContext(
				context_name.c_str(),
				Rocket::Core::Vector2i(width, height),
				_this->render_interface.get()
			)
		);

		_this->on_resize_connection =
			_this->root_window->inputs().on_resize().connect(
				std::bind(
					&Impl::on_resize, _this.get(),
					std::placeholders::_1,
					std::placeholders::_2
				)
			);

		_this->on_expose_connection =
			_this->root_window->inputs().on_expose().connect(
				std::bind(
					&Impl::on_expose, _this.get(),
					std::placeholders::_1,
					std::placeholders::_2
				)
			);

		_this->on_keydown_connection =
			_this->root_window->inputs().on_keydown().connect(
				std::bind(
					&Impl::on_key, _this.get(),
					true,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3
				)
			);
		_this->on_keyup_connection =
			_this->root_window->inputs().on_keyup().connect(
				std::bind(
					&Impl::on_key, _this.get(),
					false,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3
				)
			);

		_this->on_mousedown_connection =
			_this->root_window->inputs().on_mousedown().connect(
				std::bind(
					&Impl::on_mouse, _this.get(),
					true,
					std::placeholders::_1,
					std::placeholders::_2
				)
			);
		_this->on_mouseup_connection =
			_this->root_window->inputs().on_mouseup().connect(
				std::bind(
					&Impl::on_mouse, _this.get(),
					false,
					std::placeholders::_1,
					std::placeholders::_2
				)
			);
		_this->on_mousemove_connection =
			_this->root_window->inputs().on_mousemove().connect(
				std::bind(
					&Impl::on_mousemove, _this.get(),
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3
				)
			);
	}

	Window::Window(int width, int height)
		: Window(width, height, "default")
	{}

	Window::~Window()
	{ ETC_TRACE_DTOR(); }

	gl::renderer::Renderer& Window::renderer() ETC_NOEXCEPT
	{ return _this->root_window->renderer(); }

	gl::vector::vec2i Window::size() const ETC_NOEXCEPT
	{ return gl::vector::vec2i(this->width(), this->height()); }

	int Window::width() const ETC_NOEXCEPT
	{
		if (_this->new_size)
			return _this->new_size->x;
		return static_cast<int>(_this->root_window->renderer().viewport().w);
	}

	int Window::height() const ETC_NOEXCEPT
	{
		if (_this->new_size)
			return _this->new_size->y;
		return static_cast<int>(_this->root_window->renderer().viewport().h);
	}

	std::string const& Window::title() const ETC_NOEXCEPT
	{ return _this->root_window->title(); }

	void Window::poll()
	{
		ETC_TRACE.debug(*this, "Polling events");
		_this->root_window->poll();
		_this->rocket_context->Update();
	}

	void Window::render(gl::renderer::Painter& p)
	{
		ETC_TRACE.debug(*this, "Rendering everything");
		_this->root_window->renderer().clear(
			gl::renderer::BufferBit::depth |
			gl::renderer::BufferBit::color
		);
		if (_this->new_size)
		{
			ETC_LOG.debug(*this, "Set new size", *_this->new_size);
			_this->root_window->renderer().viewport(
				gl::viewport::Viewport(
					0, 0,
					_this->new_size->x, _this->new_size->y
				)
			);
			_this->new_size.clear();
		}
		p.push_state().lock()->ortho(0, this->width(), this->height(), 0, -10, 10);

		_this->render_interface->current_painter(p);
		_this->rocket_context->Render();
		//_this->root_window->swap_buffers();
	}

	void Window::swap_buffers()
	{ _this->root_window->swap_buffers(); }

	void Window::load_document(boost::filesystem::path const& p)
	{
		ETC_TRACE.debug("Loading document from file:", p);
		_this->rocket_context->UnloadAllDocuments();
		auto d = _this->rocket_context->LoadDocument(p.string().c_str());
		if (d == nullptr)
			throw Exception{"Couldn't load '" + p.string() + "': "};
		d->Show();
		d->RemoveReference();
	}

	void Window::load_document(std::string const& src)
	{
		ETC_TRACE.debug("Loading document from string:", etc::io::newlinesep(), src);
		_this->rocket_context->UnloadAllDocuments();
		auto d = _this->rocket_context->LoadDocumentFromMemory(src.c_str());
		if (d == nullptr)
			throw Exception{"Couldn't load '" + src + "': "};
		d->Show();
		d->RemoveReference();
	}

	void Window::add_font(boost::filesystem::path const& p)
	{
		if (!Rocket::Core::FontDatabase::LoadFontFace(p.string().c_str()))
			throw Exception{"Couldn't load font at '" + p.string() + "'"};
	}

	void Window::show()
	{ _this->root_window->show(); }

	void Window::hide()
	{ _this->root_window->hide(); }

	system::inputs::Inputs& Window::inputs()
	{ return _this->root_window->inputs(); }

	Window& Window::title(std::string const& title)
	{
		_this->root_window->title(title);
		return *this;
	}

	Rocket::Core::Input::KeyIdentifier
	Window::Impl::_to_rocket_sym(system::inputs::KeySym sym)
	{
		static Rocket::Core::Input::KeyIdentifier syms[] = {
			Rocket::Core::Input::KI_UNKNOWN, //KeySym::unknown,
			Rocket::Core::Input::KI_UNKNOWN,//KeySym::first,
			Rocket::Core::Input::KI_BACK, //KeySym::backspace,
			Rocket::Core::Input::KI_TAB, //KeySym::tab,
			Rocket::Core::Input::KI_CLEAR, //KeySym::clear,
			Rocket::Core::Input::KI_RETURN, //KeySym::return_, enter = return_,
			Rocket::Core::Input::KI_PAUSE, //KeySym::pause,
			Rocket::Core::Input::KI_ESCAPE, //KeySym::escape,
			Rocket::Core::Input::KI_SPACE, //KeySym::space,
			Rocket::Core::Input::KI_UNKNOWN,//KeySym::exclaim,
			Rocket::Core::Input::KI_OEM_7, //KeySym::quotedbl,
			Rocket::Core::Input::KI_UNKNOWN,//KeySym::hash,
			Rocket::Core::Input::KI_UNKNOWN,//KeySym::percent,
			Rocket::Core::Input::KI_UNKNOWN,//KeySym::dollar,
			Rocket::Core::Input::KI_UNKNOWN,//KeySym::ampersand,
			Rocket::Core::Input::KI_OEM_7, //KeySym::quote,
			Rocket::Core::Input::KI_UNKNOWN,//KeySym::leftparen,
			Rocket::Core::Input::KI_UNKNOWN,//KeySym::rightparen,
			Rocket::Core::Input::KI_UNKNOWN,//KeySym::asterisk,
			Rocket::Core::Input::KI_OEM_PLUS, //KeySym::plus,
			Rocket::Core::Input::KI_OEM_COMMA, //KeySym::comma,
			Rocket::Core::Input::KI_OEM_MINUS, //KeySym::minus,
			Rocket::Core::Input::KI_OEM_PERIOD, //KeySym::period,
			Rocket::Core::Input::KI_OEM_2, //KeySym::slash,
			Rocket::Core::Input::KI_0, //KeySym::k0,
			Rocket::Core::Input::KI_1, //KeySym::k1,
			Rocket::Core::Input::KI_2, //KeySym::k2,
			Rocket::Core::Input::KI_3, //KeySym::k3,
			Rocket::Core::Input::KI_4, //KeySym::k4,
			Rocket::Core::Input::KI_5, //KeySym::k5,
			Rocket::Core::Input::KI_6, //KeySym::k6,
			Rocket::Core::Input::KI_7, //KeySym::k7,
			Rocket::Core::Input::KI_8, //KeySym::k8,
			Rocket::Core::Input::KI_9, //KeySym::k9,
			Rocket::Core::Input::KI_OEM_1, //KeySym::colon,
			Rocket::Core::Input::KI_OEM_1, //KeySym::semicolon,
			Rocket::Core::Input::KI_OEM_COMMA, //KeySym::less,
			Rocket::Core::Input::KI_OEM_PLUS, //KeySym::equals,
			Rocket::Core::Input::KI_OEM_PERIOD, //KeySym::greater,
			Rocket::Core::Input::KI_OEM_2, //KeySym::question,
			Rocket::Core::Input::KI_UNKNOWN,//KeySym::at,
			Rocket::Core::Input::KI_OEM_4, //KeySym::leftbracket,
			Rocket::Core::Input::KI_OEM_5, //KeySym::backslash,
			Rocket::Core::Input::KI_OEM_6, //KeySym::rightbracket,
			Rocket::Core::Input::KI_UNKNOWN,//KeySym::caret,
			Rocket::Core::Input::KI_OEM_MINUS, //KeySym::underscore,
			Rocket::Core::Input::KI_OEM_3, //KeySym::backquote,
			Rocket::Core::Input::KI_A, //KeySym::a,
			Rocket::Core::Input::KI_B, //KeySym::b,
			Rocket::Core::Input::KI_C, //KeySym::c,
			Rocket::Core::Input::KI_D, //KeySym::d,
			Rocket::Core::Input::KI_E, //KeySym::e,
			Rocket::Core::Input::KI_F, //KeySym::f,
			Rocket::Core::Input::KI_G, //KeySym::g,
			Rocket::Core::Input::KI_H, //KeySym::h,
			Rocket::Core::Input::KI_I, //KeySym::i,
			Rocket::Core::Input::KI_J, //KeySym::j,
			Rocket::Core::Input::KI_K, //KeySym::k,
			Rocket::Core::Input::KI_L, //KeySym::l,
			Rocket::Core::Input::KI_M, //KeySym::m,
			Rocket::Core::Input::KI_N, //KeySym::n,
			Rocket::Core::Input::KI_O, //KeySym::o,
			Rocket::Core::Input::KI_P, //KeySym::p,
			Rocket::Core::Input::KI_Q, //KeySym::q,
			Rocket::Core::Input::KI_R, //KeySym::r,
			Rocket::Core::Input::KI_S, //KeySym::s,
			Rocket::Core::Input::KI_T, //KeySym::t,
			Rocket::Core::Input::KI_U, //KeySym::u,
			Rocket::Core::Input::KI_V, //KeySym::v,
			Rocket::Core::Input::KI_W, //KeySym::w,
			Rocket::Core::Input::KI_X, //KeySym::x,
			Rocket::Core::Input::KI_Y, //KeySym::y,
			Rocket::Core::Input::KI_Z, //KeySym::z,
			Rocket::Core::Input::KI_DELETE,  //KeySym::delete_, del = delete_,
			Rocket::Core::Input::KI_NUMPAD0, //KeySym::kp0,
			Rocket::Core::Input::KI_NUMPAD1, //KeySym::kp1,
			Rocket::Core::Input::KI_NUMPAD2, //KeySym::kp2,
			Rocket::Core::Input::KI_NUMPAD3, //KeySym::kp3,
			Rocket::Core::Input::KI_NUMPAD4, //KeySym::kp4,
			Rocket::Core::Input::KI_NUMPAD5, //KeySym::kp5,
			Rocket::Core::Input::KI_NUMPAD6, //KeySym::kp6,
			Rocket::Core::Input::KI_NUMPAD7, //KeySym::kp7,
			Rocket::Core::Input::KI_NUMPAD8, //KeySym::kp8,
			Rocket::Core::Input::KI_NUMPAD9, //KeySym::kp9,
			Rocket::Core::Input::KI_DECIMAL, //KeySym::kp_period,
			Rocket::Core::Input::KI_DIVIDE,      //KeySym::kp_divide,
			Rocket::Core::Input::KI_MULTIPLY,    //KeySym::kp_multiply,
			Rocket::Core::Input::KI_SUBTRACT,    //KeySym::kp_minus,
			Rocket::Core::Input::KI_ADD,         //KeySym::kp_plus,
			Rocket::Core::Input::KI_NUMPADENTER, //KeySym::kp_enter,
			Rocket::Core::Input::KI_OEM_NEC_EQUAL, //KeySym::kp_equals,
			Rocket::Core::Input::KI_UP,  //KeySym::up,
			Rocket::Core::Input::KI_DOWN, //KeySym::down,
			Rocket::Core::Input::KI_RIGHT, //KeySym::right,
			Rocket::Core::Input::KI_LEFT, //KeySym::left,
			Rocket::Core::Input::KI_INSERT, //KeySym::insert,
			Rocket::Core::Input::KI_HOME, //KeySym::home,
			Rocket::Core::Input::KI_END, //KeySym::end,
			Rocket::Core::Input::KI_PRIOR, //KeySym::pageup,
			Rocket::Core::Input::KI_NEXT, //KeySym::pagedown,
			Rocket::Core::Input::KI_F1,  //KeySym::f1,
			Rocket::Core::Input::KI_F2,  //KeySym::f2,
			Rocket::Core::Input::KI_F3,  //KeySym::f3,
			Rocket::Core::Input::KI_F4,  //KeySym::f4,
			Rocket::Core::Input::KI_F5,  //KeySym::f5,
			Rocket::Core::Input::KI_F6,  //KeySym::f6,
			Rocket::Core::Input::KI_F7,  //KeySym::f7,
			Rocket::Core::Input::KI_F8,  //KeySym::f8,
			Rocket::Core::Input::KI_F9,  //KeySym::f9,
			Rocket::Core::Input::KI_F10, //KeySym::f10,
			Rocket::Core::Input::KI_F11, //KeySym::f11,
			Rocket::Core::Input::KI_F12, //KeySym::f12,
			Rocket::Core::Input::KI_F13, //KeySym::f13,
			Rocket::Core::Input::KI_F14, //KeySym::f14,
			Rocket::Core::Input::KI_F15, //KeySym::f15,
			Rocket::Core::Input::KI_NUMLOCK, //KeySym::numlock,
			Rocket::Core::Input::KI_CAPITAL, //KeySym::capslock,
			Rocket::Core::Input::KI_SCROLL, //KeySym::scrollock,
			Rocket::Core::Input::KI_RSHIFT, //KeySym::rshift,
			Rocket::Core::Input::KI_LSHIFT, //KeySym::lshift,
			Rocket::Core::Input::KI_RCONTROL, //KeySym::rctrl,
			Rocket::Core::Input::KI_LCONTROL, //KeySym::lctrl,
			Rocket::Core::Input::KI_RMENU, //KeySym::ralt,
			Rocket::Core::Input::KI_LMENU, //KeySym::lalt,
			Rocket::Core::Input::KI_RMETA, //KeySym::rmeta,
			Rocket::Core::Input::KI_LMETA, //KeySym::lmeta,
			Rocket::Core::Input::KI_LWIN, //KeySym::lsuper,       /**< Left "Windows" key */
			Rocket::Core::Input::KI_RWIN, //KeySym::rsuper,       /**< Right "Windows" key */
			Rocket::Core::Input::KI_UNKNOWN,//KeySym::mode,         /**< "Alt Gr" key */
			Rocket::Core::Input::KI_UNKNOWN,//KeySym::compose,      /**< Multi-key compose key */
		};

		auto idx = static_cast<etc::size_type>(sym);
		ETC_ASSERT_LT(idx, (sizeof(syms) / sizeof(syms[0])));
		return syms[idx];
	}

	int Window::Impl::_to_rocket_mod(system::inputs::KeyMod mod)
	{
		using system::inputs::KeyMod;
		int state = 0;
		if (mod & KeyMod::ctrl)  state |= Rocket::Core::Input::KM_CTRL;
		if (mod & KeyMod::shift) state |= Rocket::Core::Input::KM_SHIFT;
		if (mod & KeyMod::alt)   state |= Rocket::Core::Input::KM_ALT;
		if (mod & KeyMod::meta)  state |= Rocket::Core::Input::KM_META;
		if (mod & KeyMod::caps)  state |= Rocket::Core::Input::KM_CAPSLOCK;
		if (mod & KeyMod::num)   state |= Rocket::Core::Input::KM_NUMLOCK;
		if (mod & KeyMod::mode)  state |= Rocket::Core::Input::KM_SCROLLLOCK;
		return state;
	}

}}}
