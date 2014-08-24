#ifndef  CUBE_SYSTEM_INPUTS_HPP
# define CUBE_SYSTEM_INPUTS_HPP

# include <cube/api.hpp>

# include <wrappers/boost/signals2.hpp>

# include <iosfwd>
# include <string>

namespace cube { namespace system { namespace inputs {

	enum class KeyMod : int
	{
		none    = 0x0000,
		lshift  = 0x0001,
		rshift  = 0x0002,
		lctrl   = 0x0040,
		rctrl   = 0x0080,
		lalt    = 0x0100,
		ralt    = 0x0200,
		lmeta   = 0x0400,
		rmeta   = 0x0800,
		num     = 0x1000,
		caps    = 0x2000,
		mode    = 0x4000,
		ctrl    = lctrl | rctrl,
		shift   = lshift | rshift,
		alt     = lalt | ralt,
		meta    = lmeta | rmeta,
	};

	inline
	bool
	operator &(KeyMod const l, KeyMod const r)
	{
		if ((int) l & (int) r)
			return true;
		return false;
	}

	inline
	KeyMod operator ~(KeyMod const m)
	{
		return ((KeyMod) ~((int) m));
	}

	enum class KeySym : int
	{
		unknown = 0,
		first,
		backspace,
		tab,
		clear,
		return_, enter = return_,
		pause,
		escape,
		space,
		exclaim,
		quotedbl,
		hash,
		percent,
		dollar,
		ampersand,
		quote,
		leftparen,
		rightparen,
		asterisk,
		plus,
		comma,
		minus,
		period,
		slash,
		k0,
		k1,
		k2,
		k3,
		k4,
		k5,
		k6,
		k7,
		k8,
		k9,
		colon,
		semicolon,
		less,
		equals,
		greater,
		question,
		at,
		leftbracket,
		backslash,
		rightbracket,
		caret,
		underscore,
		backquote,
		a,
		b,
		c,
		d,
		e,
		f,
		g,
		h,
		i,
		j,
		k,
		l,
		m,
		n,
		o,
		p,
		q,
		r,
		s,
		t,
		u,
		v,
		w,
		x,
		y,
		z,
		delete_, del = delete_,
		kp0,
		kp1,
		kp2,
		kp3,
		kp4,
		kp5,
		kp6,
		kp7,
		kp8,
		kp9,
		kp_period,
		kp_divide,
		kp_multiply,
		kp_minus,
		kp_plus,
		kp_enter,
		kp_equals,
		up,
		down,
		right,
		left,
		insert,
		home,
		end,
		pageup,
		pagedown,
		f1,
		f2,
		f3,
		f4,
		f5,
		f6,
		f7,
		f8,
		f9,
		f10,
		f11,
		f12,
		f13,
		f14,
		f15,
		numlock,
		capslock,
		scrollock,
		rshift,
		lshift,
		rctrl,
		lctrl,
		ralt,
		lalt,
		rmeta,
		lmeta,
		lsuper,       /**< Left "Windows" key */
		rsuper,       /**< Right "Windows" key */
		mode,         /**< "Alt Gr" key */
		compose,      /**< Multi-key compose key */
	};


    struct CUBE_API Inputs:
		private boost::noncopyable
    {
# define CUBE_SYSTEM_INPUTS_EXPOSE_SIGNAL(name, ...)                          \
	public:                                                                   \
		typedef boost::signals2::signal<void(__VA_ARGS__)> on_ ## name ## _t; \
		typedef void (on_ ## name ## _t::*on_ ## name ## _signature_t)(__VA_ARGS__);  \
	private:                                                                  \
		on_ ## name ## _t    _on_ ## name;                                    \
	public:                                                                   \
		on_ ## name ## _t&                                                    \
		on_ ## name() { return _on_ ## name; }                                \
	/**/

		CUBE_SYSTEM_INPUTS_EXPOSE_SIGNAL(expose, uint32_t, uint32_t);
		CUBE_SYSTEM_INPUTS_EXPOSE_SIGNAL(resize, uint32_t, uint32_t);
		CUBE_SYSTEM_INPUTS_EXPOSE_SIGNAL(idle, void);
		CUBE_SYSTEM_INPUTS_EXPOSE_SIGNAL(quit, void);
		CUBE_SYSTEM_INPUTS_EXPOSE_SIGNAL(keydown, KeyMod, KeySym, uint16_t);
		CUBE_SYSTEM_INPUTS_EXPOSE_SIGNAL(keyup, KeyMod, KeySym, uint16_t);
		CUBE_SYSTEM_INPUTS_EXPOSE_SIGNAL(mousemove, int32_t xrel, int32_t yrel, KeyMod);
		CUBE_SYSTEM_INPUTS_EXPOSE_SIGNAL(mousedown, uint8_t, KeyMod);
		CUBE_SYSTEM_INPUTS_EXPOSE_SIGNAL(mouseup, uint8_t, KeyMod);
		CUBE_SYSTEM_INPUTS_EXPOSE_SIGNAL(textinput, std::string const&);
# undef CUBE_SYSTEM_INPUTS_EXPOSE_SIGNAL

		~Inputs();
	};

}}} // !cube::system::inputs

CUBE_API
std::ostream&
operator <<(std::ostream& out,
            cube::system::inputs::KeyMod const mod);

CUBE_API
std::ostream&
operator <<(std::ostream& out,
            cube::system::inputs::KeySym const mod);

#endif

