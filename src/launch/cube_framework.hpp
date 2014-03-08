#ifndef  LAUNCH_CUBE_FRAMEWORK_HPP
# define LAUNCH_CUBE_FRAMEWORK_HPP

# include "version.hpp"

# include <string>

struct cube_framework
{
	std::string path;
	version v;
	cube_framework(std::string path)
		: path{path}
	{
		// XXX find out version
	}
};

#endif
