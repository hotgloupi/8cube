#ifndef  CUBE_SYSTEM_INPUTS_HPP
# define CUBE_SYSTEM_INPUTS_HPP

# include <boost/signal.hpp>

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
		return (int) l & (int) r;
	}


    struct Inputs
    {
# define CUBE_SYSTEM_INPUTS_EXPOSE_SIGNAL(name, ...)                          \
	public:                                                                   \
		typedef boost::signal<void(__VA_ARGS__)> on_ ## name ## _t;           \
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
		CUBE_SYSTEM_INPUTS_EXPOSE_SIGNAL(keydown, KeyMod mod, uint16_t unicode);
# undef CUBE_SYSTEM_INPUTS_EXPOSE_SIGNAL
	};

}}} // !cube::system::inputs

std::ostream&
operator <<(std::ostream& out,
            cube::system::inputs::KeyMod const mod);

#endif

