
#include "python/Interpreter.hpp"

#include <cube/debug/Performance.hpp>

#include <etc/init.hpp>
#include <etc/log.hpp>
#include <etc/scope_exit.hpp>
#include <etc/sys/environ.hpp>
#include <etc/test/Registry.hpp>

#include <wrappers/boost/filesystem.hpp>
#include <wrappers/boost/python.hpp>

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

int _main(int argc, char** argv)
{
	etc::Init etc_init_guard;

	{
		std::string args = "{";
		for (int i = 0; i < argc; ++i)
		{
			if (i > 0)
				args += ", ";
			args += argv[i] != nullptr ? argv[i] : "(nil)";
		}
		args += "}";
		ETC_LOG.info("Starting 8cube:", args);
	}
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
	fs::path games_dir = exec_dir.parent_path().parent_path() / "games";
	fs::path lib_dir = exec_dir.parent_path() / "lib";

	ETC_LOG("Starting 8cube");

	auto& interpreter = cubeapp::python::Interpreter::instance(lib_dir);
	ETC_SCOPE_EXIT{ cubeapp::python::Interpreter::release(); };

	fs::path python_lib_dir = lib_dir / "python";
	interpreter.setglobal("lib_dir", safe_path(python_lib_dir.string()));

	{
		int j = 0;
		boost::python::list pyargs;
		for (int i = 1; i < argc; ++i)
		{
			boost::python::str s{std::string(argv[i])};
			pyargs.append(s);
		}

		pyargs.append("-G");
		pyargs.append(games_dir.string());
		interpreter.setglobal("ARGV", pyargs);
	}

	std::string init_script =
		"import sys\n"
		"sys.path.insert(0, lib_dir)\n"
		"from cubeapp.main import main\n"
		"main(ARGV)\n"
	;
	bool success = interpreter.exec(init_script);
	return (success ? EXIT_SUCCESS : EXIT_FAILURE);
}

int main(int ac, char** av)
{
	try {
		boost::python::propagate_exception([&] { _main(ac, av); });
		return EXIT_SUCCESS;
	} catch (std::exception const& err) {
		std::cerr << "Fatal error:" << err.what() << std::endl;
	} catch (...) {
		std::cerr << "Fatal error: exiting badly...\n";
	}
	return EXIT_FAILURE;
}
