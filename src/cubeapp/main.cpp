
#include "python/Interpreter.hpp"

#include <cube/debug/Performance.hpp>

#include <etc/log.hpp>
#include <etc/scope_exit.hpp>
#include <etc/sys/environ.hpp>
#include <etc/test/Registry.hpp>

#include <wrappers/boost/filesystem.hpp>
#include <cube/python.hpp>
#include <cube/init.hpp>

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
	cube::Init cube_init_guard;

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

	boost::python::list pyargs;
	for (int i = 1; i < argc; ++i)
	{
		boost::python::str s{std::string(argv[i])};
		pyargs.append(s);
	}
	pyargs.append("-G");
	pyargs.append(games_dir.string());

	std::string init_script =
		"import sys\n"
		"sys.path.insert(0, lib_dir)\n"
		"from cubeapp.main import main\n"
	;
	interpreter.exec(init_script);
	int ret;
	cube::python::propagate_exception([&] {
		ret = boost::python::extract<int>(interpreter.globals()["main"](pyargs));
	});
	return ret;
}

int main(int ac, char* av[])
{
	try {
		return _main(ac, av);
	} catch (...) {
		std::cerr << "Fatal error: " << etc::exception::string() << std::endl;
	}
	return EXIT_FAILURE;
}

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
int CALLBACK WinMain(
    HINSTANCE   hInstance,
    HINSTANCE   hPrevInstance,
    LPSTR       lpCmdLine,
    int         nCmdShow
    )
{
	if (AttachConsole(ATTACH_PARENT_PROCESS) != 0)
	{
		FILE* ignored;
		freopen_s(&ignored, "conin$","r", stdin);
		freopen_s(&ignored, "conout$","w", stdout);
		freopen_s(&ignored, "conout$","w", stderr);
	}

	int ac;
	LPWSTR* args = ::CommandLineToArgvW(GetCommandLineW(), &ac);
	char** av = new char*[ac];
	for (int i = 0; i < ac; ++i)
	{
		int len = WideCharToMultiByte(
			CP_UTF8, // codepage
			WC_ERR_INVALID_CHARS, // fail if invalid caracters are found
			args[i],
			-1,                   // len of args[i] or -1 if null-terminated
			nullptr, // Out
			0,       // Out buffer size or ignored and return the length
			nullptr, // default character (always NULL with CP_UTF8)
			nullptr  // use the default character (bool), alway NULL when using CP_UTF8
		);
		if (len == 0)
		{
			std::wcerr << L"Couldn't parse: " << args[i] << std::endl;
			return EXIT_FAILURE;
		}
		av[i] = new char[len + 1];
		int res = WideCharToMultiByte(
			CP_UTF8, // codepage
			WC_ERR_INVALID_CHARS, // fail if invalid caracters are found
			args[i],
			-1,                   // len of args[i] or -1 if null-terminated
			av[i], // Out
			len + 1,       // Out buffer size or ignored and return the length
			nullptr, // default character (always NULL with CP_UTF8)
			nullptr  // use the default character (bool), alway NULL when using CP_UTF8
		);
		av[i][len] = '\0';
	}
	return main(ac, av);
}
#endif

