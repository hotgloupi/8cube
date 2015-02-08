#include "WindowImpl.hpp"

#include <etc/assert.hpp>

#include <atomic>

namespace cube { namespace gui { namespace window {

	Window::Impl::Impl(std::unique_ptr<system::window::Window> root_window_)
		: root_window(std::move(root_window_))
		, rocket_context(nullptr, &_delete_rocket_context)
		, new_size{etc::stack_ptr_no_init}
	{
		static std::atomic<int> count{0};
		std::string context_name = etc::to_string(
			"window", this->root_window->title(), "(", count++, ")"
		);
		this->render_interface.reset(
			new RocketRenderInterface{this->root_window->renderer()}
		);

		this->rocket_context.reset(
			Rocket::Core::CreateContext(
				context_name.c_str(),
				Rocket::Core::Vector2i(
					this->root_window->width(),
					this->root_window->height()
				),
				this->render_interface.get()
			)
		);
		this->connections = {
			this->root_window->inputs().on_resize().connect(
				std::bind(
					&Impl::on_resize, this,
					std::placeholders::_1,
					std::placeholders::_2
				)
			),
			this->root_window->inputs().on_expose().connect(
				std::bind(
					&Impl::on_expose, this,
					std::placeholders::_1,
					std::placeholders::_2
				)
			),
			this->root_window->inputs().on_keydown().connect(
				std::bind(
					&Impl::on_key, this,
					true,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3
				)
			),
			this->root_window->inputs().on_keyup().connect(
				std::bind(
					&Impl::on_key, this,
					false,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3
				)
			),
			this->root_window->inputs().on_mousedown().connect(
				std::bind(
					&Impl::on_mouse, this,
					true,
					std::placeholders::_1,
					std::placeholders::_2
				)
			),
			this->root_window->inputs().on_mouseup().connect(
				std::bind(
					&Impl::on_mouse, this,
					false,
					std::placeholders::_1,
					std::placeholders::_2
				)
			),
			this->root_window->inputs().on_mousemove().connect(
				std::bind(
					&Impl::on_mousemove, this,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3
				)
			),
			this->root_window->inputs().on_textinput().connect(
				std::bind(
					&Impl::on_textinput, this,
					std::placeholders::_1
				)
			),
		};
		this->rocket_context->AddEventListener("focus", this);
		this->rocket_context->AddEventListener("blur", this);
	}

	Window::Impl::~Impl()
	{
		for (auto& conn: this->connections)
			conn.disconnect();
	}

	void Window::Impl::on_resize(int w, int h)
	{ this->new_size.reset(w, h); }

	void Window::Impl::on_expose(int w, int h)
	{ this->new_size.reset(w, h); }

	void Window::Impl::on_key(bool down,
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

	void Window::Impl::on_textinput(std::string const& text)
	{
		ETC_TRACE.debug("Got text input", text);
		this->rocket_context->ProcessTextInput(text.c_str());
	}

	void Window::Impl::on_mouse(bool down,
	                            system::inputs::Button button,
	                            system::inputs::KeyMod mod)
	{
		ETC_TRACE.debug("Got a mouse", (down ? "press" : "release"), button, mod);
		int key_mod = _to_rocket_mod(mod);
		int rocket_button;
		switch (button)
		{
		case system::inputs::Button::unknown: rocket_button = -1; break;
		case system::inputs::Button::left: rocket_button = 0; break;
		case system::inputs::Button::right: rocket_button = 1; break;
		case system::inputs::Button::middle: rocket_button = 2; break;
		case system::inputs::Button::x1: rocket_button = 3; break;
		case system::inputs::Button::x2: rocket_button = 4; break;
		}
		if (down)
			this->rocket_context->ProcessMouseButtonDown(rocket_button, key_mod);
		else
			this->rocket_context->ProcessMouseButtonUp(rocket_button, key_mod);
	}

	void Window::Impl::on_mousemove(int32_t, int32_t, system::inputs::KeyMod mod)
	{
		auto pos = this->root_window->mouse_position();
		ETC_TRACE.debug("Got a mouse move", pos, mod);
		this->rocket_context->ProcessMouseMove(pos.x, pos.y, _to_rocket_mod(mod));
	}

	void Window::Impl::ProcessEvent(Rocket::Core::Event& event)
	{
		auto target = event.GetTargetElement();
		ETC_TRACE.debug(
		    "Got event", event.GetType().CString(),
			"on target element",
			(target ? event.GetTargetElement()->GetTagName() : "(null)")
		);
		if (auto el = event.GetTargetElement())
		{
			if (el->GetTagName() == "input" ||
			    el->GetTagName() == "textarea")
			{
				if (event.GetType() == "focus")
					this->root_window->start_text_input();
				else if (event.GetType() == "blur")
					this->root_window->stop_text_input();
			}
		}
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
			Rocket::Core::Input::KI_LWIN, //KeySym::lsuper,
			Rocket::Core::Input::KI_RWIN, //KeySym::rsuper,
			Rocket::Core::Input::KI_UNKNOWN,//KeySym::mode,
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
