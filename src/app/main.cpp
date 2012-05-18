
#include <iostream>
#include <stdexcept>
#include <string>

#include <boost/algorithm/string/predicate.hpp>
#include <wrappers/boost/filesystem.hpp>
#include <wrappers/windows.hpp>

#include "python/Interpreter.hpp"

namespace fs = boost::filesystem;
namespace algo = boost::algorithm;

int main(int argc, char* argv[])
{
	if (argc < 1 || argv[0] == nullptr)
		throw std::runtime_error("Wrong program inputs");

	fs::path exec_dir = fs::absolute(argv[0]).parent_path();
	fs::path lib_dir = exec_dir.parent_path().append("lib", fs::path::codecvt());

#ifdef _WIN32
	{
		fs::directory_iterator dir(exec_dir), end;
		for (; dir != end; ++dir)
		{
			fs::path p(*dir);
			if (algo::ends_with(p.string(), ".dll"))
			{
				std::cout << "-- Load library: " << p.string() << std::endl;
				::LoadLibrary(p.string().c_str());
			}
		}
	}
#endif

	fs::path python_lib_dir = lib_dir.append("python", fs::path::codecvt());
	app::python::interpreter.setglobal("lib_dir", python_lib_dir.string());

	std::string init_script =
		"import sys\nsys.path.insert(0, lib_dir)\n"
		"from cube.main import main\n"
		"main()\n"
	;

	return !app::python::interpreter.exec(init_script);
}
