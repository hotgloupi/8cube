#ifndef  CUBE_SYSTEM_INPUTS_HPP
# define CUBE_SYSTEM_INPUTS_HPP

# include <boost/signal.hpp>

namespace cube { namespace system { namespace inputs {

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
# undef CUBE_SYSTEM_INPUTS_EXPOSE_SIGNAL
	};

}}} // !cube::system::inputs

#endif

