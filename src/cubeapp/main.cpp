
#include "python/Interpreter.hpp"

#include <cube/main_proto.hpp>
#include <cube/debug/Performance.hpp>

#include <etc/log.hpp>

#include <wrappers/boost/filesystem.hpp>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <iostream>
#include <stdexcept>
#include <string>

namespace fs = boost::filesystem;
namespace algo = boost::algorithm;

static int load_libraries(fs::path lib_dir);

static std::string safe_path(std::string const& path)
{
	std::string res{path};
	std::replace(res.begin(), res.end(), '\\', '/');
	return res;
}

CUBE_MAIN_PROTO(int argc, char** argv)
{
	if (argc < 1 || argv[0] == nullptr)
	{
		std::cerr << "Wrong program arguments\n";
		return EXIT_FAILURE;
	}

	fs::path exec_dir = fs::canonical(argv[0]).parent_path();
	fs::path games_dir = exec_dir.parent_path() / "share" / "8cube" / "games";
	fs::path lib_dir = exec_dir.parent_path() / "lib";

#ifdef _WIN32
	//load_libraries(exec_dir);
	load_libraries(lib_dir);
#else
	(void)load_libraries;
#endif

	ETC_LOG_COMPONENT("cubeapp.main");

	ETC_LOG("Starting 8cube");

	auto& interpreter = cubeapp::python::Interpreter::instance(lib_dir);

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
		int res = interpreter.exec(init_script);
		cube::debug::Performance::instance().dump();
		return res != 0;
	} catch (std::exception const& err) {
		std::cerr << "Fatal error:" << err.what() << std::endl;
	} catch (...) {
		std::cerr << "Fatal error: exiting badly...\n";
	}
	std::cerr << "The 'impossible' happened, you should report this to contact@8cube.io\n";
	return 1;
}

#ifdef _WIN32
# include <wrappers/windows.hpp>
#else
# include <dlfcn.h>
#endif

static int load_libraries(fs::path lib_dir)
{
	fs::directory_iterator dir(lib_dir), end;
	for (; dir != end; ++dir)
	{
		fs::path p(*dir);
		//std::cout << "-- file: " << p.string() << std::endl;
		if (algo::ends_with(p.string(), ".dll") ||
		    algo::ends_with(p.string(), ".pyd") ||
		    algo::ends_with(p.string(), ".so"))
		{
#ifdef _WIN32
			std::cout << "-- Load library: " << p.string() << std::endl;
			::LoadLibrary(p.string().c_str());
#else
      //auto handle = ::dlopen(p.string().c_str(), RTLD_NOW);
      //if (!handle)
      //  {
      //    std::cerr << "Error while loading " << p.string() << ": " << dlerror() << std::endl;
      //  }
      //  if (auto res = dlsym(handle, "__caca0_free_bitmap"))
      //    std::cout << "found: " << (void const*)res << std::endl;
#endif
		}
	}
	return 0;
}
