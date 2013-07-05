#ifndef  CUBE_SYSTEM_INPUTS_HPP
# define CUBE_SYSTEM_INPUTS_HPP

# include <cube/api.hpp>

# include <wrappers/boost/signals2.hpp>

# include <iosfwd>

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
		first = 0,
		backspace = 8,
		tab = 9,
		clear = 12,
		return_ = 13, enter = return_,
		pause = 19,
		escape = 27,
		space = 32,
		exclaim = 33,
		quotedbl = 34,
		hash = 35,
		dollar = 36,
		ampersand = 38,
		quote = 39,
		leftparen = 40,
		rightparen = 41,
		asterisk = 42,
		plus = 43,
		comma = 44,
		minus = 45,
		period = 46,
		slash = 47,
		k0 = 48,
		k1 = 49,
		k2 = 50,
		k3 = 51,
		k4 = 52,
		k5 = 53,
		k6 = 54,
		k7 = 55,
		k8 = 56,
		k9 = 57,
		colon = 58,
		semicolon = 59,
		less = 60,
		equals = 61,
		greater = 62,
		question = 63,
		at = 64,
		leftbracket = 91,
		backslash = 92,
		rightbracket = 93,
		caret = 94,
		underscore = 95,
		backquote = 96,
		a = 97,
		b = 98,
		c = 99,
		d = 100,
		e = 101,
		f = 102,
		g = 103,
		h = 104,
		i = 105,
		j = 106,
		k = 107,
		l = 108,
		m = 109,
		n = 110,
		o = 111,
		p = 112,
		q = 113,
		r = 114,
		s = 115,
		t = 116,
		u = 117,
		v = 118,
		w = 119,
		x = 120,
		y = 121,
		z = 122,
		delete_ = 127, del = delete_,
		kp0 = 256,
		kp1 = 257,
		kp2 = 258,
		kp3 = 259,
		kp4 = 260,
		kp5 = 261,
		kp6 = 262,
		kp7 = 263,
		kp8 = 264,
		kp9 = 265,
		kp_period = 266,
		kp_divide = 267,
		kp_multiply = 268,
		kp_minus = 269,
		kp_plus = 270,
		kp_enter = 271,
		kp_equals = 272,
		up = 273,
		down = 274,
		right = 275,
		left = 276,
		insert = 277,
		home = 278,
		end = 279,
		pageup = 280,
		pagedown = 281,
		f1 = 282,
		f2 = 283,
		f3 = 284,
		f4 = 285,
		f5 = 286,
		f6 = 287,
		f7 = 288,
		f8 = 289,
		f9 = 290,
		f10 = 291,
		f11 = 292,
		f12 = 293,
		f13 = 294,
		f14 = 295,
		f15 = 296,
		numlock = 300,
		capslock = 301,
		scrollock = 302,
		rshift = 303,
		lshift = 304,
		rctrl = 305,
		lctrl = 306,
		ralt = 307,
		lalt = 308,
		rmeta = 309,
		lmeta = 310,
		lsuper = 311,       /**< Left "Windows" key */
		rsuper = 312,       /**< Right "Windows" key */
		mode = 313,         /**< "Alt Gr" key */
		compose = 314,      /**< Multi-key compose key */
	};


    struct CUBE_API Inputs : private boost::noncopyable
    {
# define CUBE_SYSTEM_INPUTS_EXPOSE_SIGNAL(name, ...)                          \
	public:                                                                   \
		typedef boost::signals2::signal<void(__VA_ARGS__)> on_ ## name ## _t; \
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
		CUBE_SYSTEM_INPUTS_EXPOSE_SIGNAL(mousemove, int32_t xrel, int32_t yrel);
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

