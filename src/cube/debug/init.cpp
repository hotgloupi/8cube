#include "init.hpp"
#include "Performance.hpp"

#include <etc/sys/environ.hpp>

namespace cube { namespace debug {

	Init::Init()
	{}

	Init::~Init()
	{
		Performance::instance().shutdown();
		if (etc::sys::environ::try_as<bool>("CUBE_DEBUG_PERFORMANCE_DUMP"))
			Performance::instance().dump();
	}

}}
