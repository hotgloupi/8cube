
#include <iostream>
#include <stdexcept>
#include <string>

#include <wrappers/boost/filesystem.hpp>

#include "python/Interpreter.hpp"

namespace fs = boost::filesystem;

int main(int argc, char* argv[])
{
	if (argc < 1 || argv[0] == nullptr)
		throw std::runtime_error("Wrong program inputs");

	fs::path exec_dir = fs::absolute(argv[0]).parent_path();
	fs::path lib_dir = exec_dir.parent_path().append("lib", fs::path::codecvt());
	fs::path python_lib_dir = lib_dir.append("python", fs::path::codecvt());

	std::string init_script =
		"import sys\nsys.path = [lib_dir]\n"
		"print(sys.path)\n"
	;

	app::python::interpreter.setglobal("lib_dir", python_lib_dir.string());

	return !app::python::interpreter.exec(init_script);
}
