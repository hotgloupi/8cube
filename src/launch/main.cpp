/**
 * Standalone program that prepare everything and launch a game.
 *
 * The program is compiled for every game on every platform.
 * The environment variable GAME_ID defines the target game id (its name).
 *
 *
 * Scenarios:
 *  - SELF is in the game directory.
 *  - SELF is anywhere but in the game.
 *
 * ## Launch process without internet:
 *
 *  (Launch game.exe) -> <Has --update-launcher flag ?>
 *                        NO       |     YES
 *  <has internet ?>  <------------/\----------> (COPY game.new.exe to game.exe)
 *      |                                                   |
 *      | NO                                                V
 *      |                                             (Launch game.exe)
 *      V           NO
 *  <has cube ?>  -----> (ERROR)
 *      |
 *      | YES
 *      V                NO
 *  <has the game ?>   ------> (ERROR)
 *      |
 *      | YES
 *      V                                    NO
 *  <Does version matches (compatibility)>  ----> (ERROR)
 *      |
 *      | YES
 *      V
 *  (LAUNCH selected cube version with the game)
 *
 *
 *  ## Launch process with internet
 *
 *  (Launch game.exe) -> <Has --update-launcher flag ?>
 *                        NO       |     YES
 *  <has internet ?>  <------------/\----------> (COPY game.new.exe to game.exe)
 *      |                                                   |
 *      | YES                                               V
 *      |                                             (Launch game.exe)
 *      V                         NO
 *  <Is the game up-to-date ?>  ------> (UPDATE THE GAME) ----> (LAUNCH game.new.exe --update-launcher)
 *      |
 *      | YES
 *      V                                      NO
 *  <Is lastest compatible cube present> --------------> (FETCH CUBE VERSION)
 *      |                                                       |
 *      | YES                                                   |
 *      V                                                       |
 *  (LAUNCH selected cube version with the game) <--------------/
 */

#include "version.hpp"
#include "cube_game.hpp"
#include "cube_framework.hpp"

#include <etc/sys/environ.hpp>
#include <etc/path.hpp>
#include <etc/log.hpp>
#include <etc/scope_exit.hpp>

#include <wrappers/boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include <boost/preprocessor/stringize.hpp>

#include <algorithm>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <fstream>

#ifndef _WIN32
# include <sys/stat.h> // chmod
#endif

#define GAME_ID_STRING BOOST_PP_STRINGIZE(GAME_ID)

#ifndef GAME_ID
# error GAME_ID must be defined
#endif

#ifdef major
# undef major
#endif
#ifdef minor
# undef minor
#endif

namespace env = etc::sys::environ;

namespace {

	ETC_LOG_COMPONENT("launch." GAME_ID_STRING);

/**
 *  Variables that influences the launcher
 *   - CUBE_ROOT: The root directory of cube frameworks (different versions).
 *   - CUBE_VERSION: Use a specific version.
 *   - CUBE_GAMES_ROOT: Where to store the games
 *   - CUBE_RELEASE_SERVER: Adresse used to fetch cube and games.
 *
 *   See below for their default value.
 */
#define DEFINE_VAR(name, default_value) \
std::string const& name() \
{ \
	static std::string var = [] () -> std::string { \
		if (env::contains(#name)) \
			ETC_LOG.debug("Env var '" #name "' set: '" \
		         , env::get(#name) , "'"); \
		return env::get(#name, default_value); \
	}(); \
	return var; \
} \
/**/

DEFINE_VAR(CUBE_ROOT            , "~/.cube");
DEFINE_VAR(CUBE_VERSION         , "");
DEFINE_VAR(CUBE_GAMES_ROOT      , etc::path::absolute(CUBE_ROOT(), CUBE_VERSION(), "games"));
DEFINE_VAR(CUBE_RELEASE_SERVER  , "release.cube.io");
#undef DEFINE_VAR

// Set at startup
std::string& PROGRAM_PATH() { static std::string s; return s; }

bool has_internet()
{
    return false;
}

#ifdef _WIN32
# define CUBE_BINARY_NAME "8cube.exe"
#else
# define CUBE_BINARY_NAME "8cube"
#endif
#define CUBE_BINARY etc::path::join("bin", CUBE_BINARY_NAME)

// Return sorted available cube installations (most up-to-date firsts).
std::vector<cube_framework> find_cube_frameworks()
{
	std::vector<cube_framework> res{};
	auto inspect_dir = [&] (std::string const& directory) {
		boost::system::error_code ec;
		fs::directory_iterator it(directory, ec), end;
		if (ec)
		{
			ETC_TRACE.debug("Invalid cube root directory '", directory, "': "
			         ,  ec.message(), " (", ec, ")");
		}
		for (; it != end; ++it)
		{
			ETC_TRACE.debug("Checking: ", it->path());
			auto bin = it->path() / CUBE_BINARY;
			if (fs::exists(bin))
			{
				ETC_TRACE.debug("Found cube framework ", bin);
				res.emplace_back(bin.string());
			}
		}
	};
	if (!CUBE_VERSION().empty())
	{
		std::string path = etc::path::absolute(
			CUBE_ROOT(),
			CUBE_VERSION(),
			CUBE_BINARY
		);
		ETC_TRACE.debug("Using cube framework '", path, "'");
		res.emplace_back(path);
	}
	else
	{
		ETC_TRACE.debug("First inspect cube root default: '", CUBE_ROOT()
		         , "'");
		inspect_dir(CUBE_ROOT());
	}

	if (res.empty())
	{
		fs::path path = etc::path::absolute(etc::path::directory_name(PROGRAM_PATH()), "..", "..");
		ETC_TRACE.debug("Trying cube root at ", path);
		inspect_dir(path.string());
	}

	std::sort(
		res.begin(),
		res.end(),
		[] (cube_framework const& l, cube_framework const& r) { return r.v < l.v; }
	);
	return res;
}

cube_game find_game()
{
	auto inspect_game_dir = [&] (fs::path const& dir) -> std::string {
		boost::system::error_code ec;
		fs::directory_iterator it(dir, ec), end;
		if (!ec)
			for (; it != end; ++it)
			{
				if (it->path().filename() == GAME_ID_STRING)
					return it->path().string();
			}
		return std::string{};
	};
	std::string path;
	path = inspect_game_dir(
		etc::path::join(etc::path::directory_name(PROGRAM_PATH()), "..")
	);
	if (path.empty())
		path = inspect_game_dir(CUBE_GAMES_ROOT());

	ETC_TRACE.debug("Return game at '", path, "'");
	return cube_game(path, GAME_ID_STRING);
}

void launch_game(std::vector<cube_framework> const& frameworks, cube_game const& g)
{
	if (frameworks.empty())
		throw std::runtime_error("Empty framework list");

	// TODO version match
	auto const& framework = frameworks[0];
	auto cmd = framework.path + " " + g.path;
	::system(cmd.c_str());
}

void with_internet()
{
}

void without_internet()
{
    auto cube_frameworks = find_cube_frameworks();
    if (cube_frameworks.empty())
        throw std::runtime_error{"Cube framework not found"};

    auto game = find_game();

    launch_game(cube_frameworks, game);
}

boost::filesystem::path default_temp_dir()
{
#ifdef _WIN32
	TCHAR buf [MAX_PATH];

	if (GetTempPath (MAX_PATH, buf) != 0)
		// XXX DO something useful
#else
	return etc::sys::environ::get("TMPDIR", "/tmp");
#endif
}

template<typename Stream>
struct temporary_file
{
public:
	typedef boost::filesystem::path path_t;
	typedef Stream stream_t;
private:
	path_t _dir;
	std::string _pattern;
	path_t _path;
	stream_t _stream;


public:
	temporary_file(path_t const& dir, std::string const& pattern)
		: _dir{dir}
		, _pattern{pattern}
		, _path(_gen_unique_path())
		, _stream{_path.string()}
	{}

	temporary_file(std::string const& pattern)
		: temporary_file{default_temp_dir(), pattern}
	{}

	temporary_file()
		: temporary_file{default_temp_dir(), std::string{}}
	{}

	~temporary_file()
	{
		_stream.close();
		ETC_TRACE.debug("Closing: ", _path);
		boost::system::error_code ec;
		boost::filesystem::remove(_path, ec);
		if (ec)
			ETC_TRACE.debug("Couldn't remove temporary file ", _path);
	}
public:
	path_t const& path() const { return _path; }
	stream_t& stream() { return _stream; }

private:
	path_t _gen_unique_path()
	{
		boost::filesystem::path p;
		do {
			if (_pattern.empty())
				p = _dir / boost::filesystem::unique_path();
			else
				p = _dir / boost::filesystem::unique_path(_pattern);
		} while (boost::filesystem::exists(p));
		return p;
	}
};

} // ! anonymous


int main(int argc, char const* av[])
{
	ETC_SCOPE_EXIT{ etc::log::shutdown(); };
	PROGRAM_PATH() = av[0];
	if (!etc::sys::environ::contains("CUBE_LAUNCH_BOUNCER"))
	{
		ETC_TRACE.debug("Creating temporary launcher");
		etc::sys::environ::set("CUBE_LAUNCH_BOUNCER", PROGRAM_PATH());
		std::ifstream src{PROGRAM_PATH()};
		temporary_file<std::ofstream> out{"8cube-" GAME_ID_STRING "-%%%%.exe"};
		ETC_TRACE.debug(PROGRAM_PATH() , "->>",  out.path());
		char buf[4096];
		while (!src.eof())
		{
			src.read(buf, sizeof(buf));
			std::streamsize read_bytes = src.gcount();
			if (read_bytes > 0)
				out.stream().write(buf, read_bytes);
		}
		src.close();
		out.stream().close();

#ifndef _WIN32
		chmod(out.path().string().c_str(), S_IRUSR | S_IXUSR);
#endif
		system(out.path().string().c_str());

		return 0;
	}
	PROGRAM_PATH() = etc::sys::environ::get("CUBE_LAUNCH_BOUNCER");
	try
	{
		if (has_internet())
		{
			try
			{
				with_internet();
				return 0;
			}
			catch (std::exception const& err)
			{
				ETC_TRACE.warn(
					"Couldn't launch the game using internet:", err.what()
				);
			}
		}
		without_internet();
	}
	catch (std::exception const& err)
	{
		ETC_TRACE.error("Fatal error:",  err.what());
		return 1;
	}
	return 0;
}
