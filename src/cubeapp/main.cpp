
#include "python/Interpreter.hpp"

#include <cube/debug/Performance.hpp>

#include <etc/init.hpp>
#include <etc/log.hpp>
#include <etc/scope_exit.hpp>
#include <etc/sys/environ.hpp>
#include <etc/test/Registry.hpp>

#include <wrappers/boost/filesystem.hpp>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <iostream>
#include <stdexcept>
#include <string>

namespace fs = boost::filesystem;
namespace algo = boost::algorithm;

static std::string safe_path(std::string const& path)
{
	std::string res{path};
	std::replace(res.begin(), res.end(), '\\', '/');
	return res;
}

ETC_LOG_COMPONENT("cubeapp.main");

int main(int argc, char** argv)
{
	etc::Init etc_init_guard;
	ETC_SCOPE_EXIT{
		cube::debug::Performance::instance().shutdown();
		if (etc::sys::environ::get("CUBE_PERFORMANCE_DUMP", "").size())
			cube::debug::Performance::instance().dump();
	};

	for (int i = 1; i < argc; i++)
	{
		if (argv[i] == std::string{"--etc-tests"})
		{
			if (!etc::test::registry().run())
				return EXIT_FAILURE;
			return EXIT_SUCCESS;
		}
	}

	if (argc < 1 || argv[0] == nullptr)
	{
		std::cerr << "Wrong program arguments\n";
		return EXIT_FAILURE;
	}

	fs::path exec_dir = fs::canonical(argv[0]).parent_path();
	fs::path games_dir = exec_dir.parent_path() / "share" / "8cube" / "games";
	fs::path lib_dir = exec_dir.parent_path() / "lib";

	ETC_LOG("Starting 8cube");

	auto& interpreter = cubeapp::python::Interpreter::instance(lib_dir);
	ETC_SCOPE_EXIT{ cubeapp::python::Interpreter::release(); };

	fs::path python_lib_dir = lib_dir / "python";
	interpreter.setglobal("lib_dir", safe_path(python_lib_dir.string()));

	// XXX This, is ugly and not safe.
	std::string pyargs = "[\n";
	for (int i = 1; i < argc; ++i)
	{
		std::string arg = argv[i];
		algo::replace_all(arg, "'''", "\\'\\'\\'");

		pyargs += "\t'''" + arg + "''',\n";
	}
	pyargs += "\t'-G',\n";
	pyargs += "\t'''" + safe_path(games_dir.string()) + "''',\n";
	pyargs += "]";

	std::string init_script =
		"import sys\n"
		"sys.path.insert(0, lib_dir)\n"
		"from cubeapp.main import main\n"
		"main(" + pyargs + ")\n"
	;
	try {
		bool success = interpreter.exec(init_script);
		return (success ? EXIT_SUCCESS : EXIT_FAILURE);
	} catch (std::exception const& err) {
		std::cerr << "Fatal error:" << err.what() << std::endl;
	} catch (...) {
		std::cerr << "Fatal error: exiting badly...\n";
	}
	std::cerr << "The 'impossible' happened, you should report this to contact@8cube.io\n";
	return EXIT_FAILURE;
}
