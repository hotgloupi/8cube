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
#include "config.hpp"

#include <etc/temp/File.hpp>
#include <etc/sys/environ.hpp>
#include <etc/path.hpp>
#include <etc/log.hpp>
#include <etc/scope_exit.hpp>
#include <etc/init.hpp>

#include <wrappers/windows.hpp>
#include <wrappers/boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

namespace fs = boost::filesystem;
namespace algo = boost::algorithm;

#include <algorithm>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <fstream>

#ifndef _WIN32
# include <sys/stat.h> // chmod
# include <sys/wait.h> // waitpid
#else
# include <process.h> // execve
#endif

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

	ETC_LOG_COMPONENT("launch.main");

bool has_internet(config const&)
{
    return false;
}


void spawn_process(std::string exe, std::vector<std::string> args = {})
{
	ETC_LOG.debug("Launching command:", exe);
#ifdef _WIN32
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// XXX windows sucks, so do this quoting pass (which is incorrect if some
	// path contains a quote).
	exe = "\"" + exe + "\"";
	for (auto arg: args)
		exe += " \"" + arg + "\"";

	// Start the child process.
	if (!CreateProcess(
		NULL,           // No module name (use command line)
		(LPSTR)exe.c_str(),    // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
	   )
	{
		std::string err = std::to_string(GetLastError());
		throw std::runtime_error("CreateProcess failed: " + err);
	}

	// Wait until child process exits.
	WaitForSingleObject( pi.hProcess, INFINITE );

	// Close process and thread handles.
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
#else
	pid_t pid = fork();
	if (pid < 0)
		throw std::runtime_error("Couldn't fork");
	if (pid == 0)
	{
		char const** argv = (char const**)calloc(sizeof(char*), args.size() + 2);
		argv[0] = exe.c_str();
		for (size_t i = 0; i < args.size(); i++)
			argv[i + 1] = args[i].c_str();
		argv[args.size() + 1] = nullptr;
		execv(exe.c_str(), (char**)argv);
	}
	else
	{
		int status;
		waitpid(pid, &status, 0);
	}
#endif
}

#ifdef _WIN32
# define CUBE_BINARY_NAME "8cube.exe"
#else
# define CUBE_BINARY_NAME "8cube"
#endif
#define CUBE_BINARY etc::path::join("bin", CUBE_BINARY_NAME)

void inspect_dir(std::vector<cube_framework>& res, std::string const& directory) {
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
}

// Return sorted available cube installations (most up-to-date firsts).
std::vector<cube_framework> find_cube_frameworks(config const& cfg)
{
	std::vector<cube_framework> res;

	if (!cfg.cube_version().empty())
	{
		std::string path = etc::path::absolute(
			cfg.cube_root(),
			cfg.cube_version(),
			CUBE_BINARY
		);
		ETC_TRACE.debug("Using cube framework '", path, "'");
		res.emplace_back(path);
	}
	else
	{
		ETC_TRACE.debug("First inspect cube root default: '", cfg.cube_root()
		         , "'");
		inspect_dir(res, cfg.cube_root());
	}

	if (res.empty())
	{
		fs::path path = etc::path::absolute(
			etc::path::directory_name(cfg.program_path()), "..", "..");
		ETC_TRACE.debug("Trying cube root at ", path);
		inspect_dir(res, path.string());
	}

	std::sort(
		res.begin(),
		res.end(),
		[] (cube_framework const& l, cube_framework const& r) { return r.v < l.v; }
	);
	return res;
}


cube_game find_game(config const& cfg)
{
	auto inspect_game_dir = [&] (fs::path const& dir) -> std::string {
		ETC_TRACE.debug("Inspect game directory", dir);
		boost::system::error_code ec;
		fs::directory_iterator it(dir, ec), end;
		if (!ec)
			for (; it != end; ++it)
			{
				if (it->path().filename() == cfg.game_id())
				{
					ETC_LOG.debug("Found", it->path());
					return it->path().string();
				}
				ETC_LOG.debug("Ignore path", it->path());
			}
		return std::string{};
	};
	std::string path;
	path = inspect_game_dir(
		etc::path::join(etc::path::directory_name(cfg.program_path()), "..", "games")
	);
	if (path.empty())
		path = inspect_game_dir(cfg.cube_games_root());

	ETC_TRACE.debug("Return game at '", path, "'");
	return cube_game(path, cfg.game_id());
}

void launch_game(std::vector<cube_framework> const& frameworks, cube_game const& g)
{
	if (frameworks.empty())
		throw std::runtime_error("Empty framework list");

	// TODO version match
	auto const& framework = frameworks[0];
	auto cmd = framework.path;
	spawn_process(cmd, {g.path});
}

void with_internet(config const&)
{
}

void without_internet(config const& cfg)
{
    auto cube_frameworks = find_cube_frameworks(cfg);
    if (cube_frameworks.empty())
        throw std::runtime_error{"Cube framework not found"};

    auto game = find_game(cfg);

    launch_game(cube_frameworks, game);
}


} // ! anonymous

int main(int argc, char const* av[])
{
	etc::Init etc_init_guard;


	if (!etc::sys::environ::contains("CUBE_LAUNCH_BOUNCER"))
	{
		config cfg(av[0]);
		ETC_TRACE.debug("Creating temporary launcher");
		etc::sys::environ::set("CUBE_LAUNCH_BOUNCER", cfg.program_path());
		std::ifstream src{cfg.program_path(), std::ios_base::binary};
		etc::temp::File<std::ofstream> out{
			etc::temp::default_directory(),
			"8cube-" + cfg.game_id() + "-%%%%.temp",
			std::ios_base::trunc | std::ios_base::out | std::ios_base::binary
		};
		if (!out.stream().good())
		{
			ETC_LOG.error("Couldn't open temporary file", out.path());
			return 1;
		}
		ETC_LOG.debug(cfg.program_path() , "->>",  out.path());
		char buf[4096];
		size_t written = 0;
		while (!src.eof() && out.stream().good())
		{
			src.read(buf, sizeof(buf));
			std::streamsize read_bytes = src.gcount();
			if (read_bytes > 0)
			{
				out.stream().write(buf, read_bytes);
				written += read_bytes;
			}
		}
		src.close();
		out.stream().close();
		ETC_LOG.debug(out.path(), "of", written, "bytes created");

		std::string bin = out.path().string();
#ifndef _WIN32
		chmod(bin.c_str(), S_IRUSR | S_IXUSR);
#else
		bin += ".exe";
		boost::system::error_code ec;
		boost::filesystem::rename(out.path(), bin, ec);
		if (ec)
			ETC_LOG.error("Couldn't create the executable", bin, ":", ec, ec.message());
#endif
		spawn_process(bin);
		return 0;
	}
	config cfg(etc::sys::environ::get("CUBE_LAUNCH_BOUNCER"));
	try
	{
		if (has_internet(cfg))
		{
			try
			{
				with_internet(cfg);
				return 0;
			}
			catch (std::exception const& err)
			{
				ETC_TRACE.warn(
					"Couldn't launch the game using internet:", err.what()
				);
			}
		}
		without_internet(cfg);
	}
	catch (std::exception const& err)
	{
		ETC_TRACE.error("Fatal error:",  err.what());
		return 1;
	}
	return 0;
}
