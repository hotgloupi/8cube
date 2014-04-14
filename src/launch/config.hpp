#ifndef  LAUNCHER_CONFIG_HPP
# define LAUNCHER_CONFIG_HPP

#include "env.hpp"

#include <etc/log.hpp>
#include <etc/path.hpp>

#include <boost/preprocessor/stringize.hpp>

#include <string>


/**
 * State class.
 *  Variables that influences the config
 *   - CUBE_ROOT: The root directory of cube frameworks (different versions).
 *   - CUBE_VERSION: Use a specific version.
 *   - CUBE_GAMES_ROOT: Where to store the games
 *   - CUBE_RELEASE_SERVER: Adresse used to fetch cube and games.
 *
 *   See below for their default value.
 */
struct config
{
	ETC_LOG_COMPONENT("launch.config");

	std::string _program_path;
	std::string _game_id;
	std::string _cube_root;
	std::string _cube_version;
	std::string _cube_games_root;
	std::string _cube_release_server;

	config(std::string program_path)
		: _program_path(std::move(program_path))
		, _game_id(env::get("GAME_ID", BOOST_PP_STRINGIZE(GAME_ID)))
		, _cube_root(env::get("CUBE_ROOT", "~/.cube"))
		, _cube_version(env::get("CUBE_VERSION", ""))
		, _cube_games_root{
			env::get(
				"CUBE_GAMES_ROOT",
				etc::path::absolute(_cube_root, "games")
			)
		}
		, _cube_release_server(env::get("CUBE_RELEASE_SERVER", "dl.8cube.io"))
	{
		ETC_LOG.debug(*this, "CUBE_ROOT =", _cube_root);
		ETC_LOG.debug(*this, "CUBE_VERSION =", _cube_version);
		ETC_LOG.debug(*this, "CUBE_GAMES_ROOT =", _cube_games_root);
		ETC_LOG.debug(*this, "CUBE_RELEASE_SERVER =", _cube_release_server);
	}

	std::string const& program_path() const { return _program_path; }
	std::string const& game_id() const { return _game_id; }
	std::string const& cube_root() const { return _cube_root; }
	std::string const& cube_version() const { return _cube_version; }
	std::string const& cube_games_root() const { return _cube_games_root; }
	std::string const& cube_release_server() const { return _cube_release_server; }
};

#endif
