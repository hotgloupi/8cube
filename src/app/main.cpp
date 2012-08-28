
#include <iostream>
#include <stdexcept>
#include <string>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <wrappers/boost/filesystem.hpp>
#include <wrappers/opengl.hpp>

#include "python/Interpreter.hpp"

namespace fs = boost::filesystem;
namespace algo = boost::algorithm;

static int load_libraries(fs::path lib_dir);

int main(int argc, char* argv[])
{
	if (argc < 1 || argv[0] == nullptr)
		throw std::runtime_error("Wrong program inputs");

	fs::path exec_dir = fs::absolute(argv[0]).parent_path();
	fs::path lib_dir = exec_dir.parent_path().append("lib", fs::path::codecvt());

	load_libraries(exec_dir);
	load_libraries(lib_dir);

	auto& interpreter = app::python::Interpreter::instance();

	fs::path python_lib_dir = lib_dir.append("python", fs::path::codecvt());
	interpreter.setglobal("lib_dir", python_lib_dir.string());

	// XXX This, is ugly and not safe.
	std::string pyargs = "[";
	for (int i = 1; i < argc; ++i)
	{
		std::string arg = argv[i];
		algo::replace_all(arg, "\\", "\\\\");
		algo::replace_all(arg, "\"", "\\\"");

		pyargs += "\"" + arg + "\",";
	}
	pyargs += "]";

	std::string init_script =
		"import sys\nsys.path.insert(0, lib_dir)\n"
		"from cube.main import main\n"
		"import cube\n"
		"main(" + pyargs + ")\n"
	;
	return !interpreter.exec(init_script);
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
        algo::ends_with(p.string(), ".so"))
    {
      std::cout << "-- Load library: " << p.string() << std::endl;
#ifdef _WIN32
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
