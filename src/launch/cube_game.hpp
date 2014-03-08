#ifndef  LAUNCH_CUBE_GAME_HPP
# define LAUNCH_CUBE_GAME_HPP

# include "version.hpp"

# include <string>
# include <vector>

struct cube_game
{
	std::string path;
    std::string name;
    std::vector<version> min_cube_version;
    std::vector<version> max_cube_version;

    cube_game(std::string path, std::string name)
		: path{path}
		, name{name}
    {
        // XXX find versions here
    }

    cube_game(cube_game const&) = default;
};


#endif
