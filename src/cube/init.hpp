#ifndef  CUBE_INIT_HPP
# define CUBE_INIT_HPP

# include <cube/api.hpp>

# include <etc/init.hpp>
# include <cube/debug/init.hpp>
# include <cube/gui/init.hpp>

namespace cube {

	// Initialize and shutdown cube library.
	struct CUBE_API Init
	{
		Init();
		~Init();

		etc::Init etc_guard;
		debug::Init debug_guard;
		gui::Init gui_guard;
	};

}

#endif
