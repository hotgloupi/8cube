#include <cube/python.hpp>
#include <boost/python/import.hpp>
#include <boost/algorithm/string.hpp>

#include "Interpreter.hpp"
#ifdef environ
# undef environ
#endif
#include <etc/assert.hpp>
#include <etc/log.hpp>
#include <etc/platform.hpp>
#include <etc/scope_exit.hpp>
#include <etc/sys/environ.hpp>

#include <iostream>
#include <stdexcept>

namespace py = boost::python;
namespace fs = boost::filesystem;

ETC_LOG_COMPONENT("cubeapp.python.Interpreter");

namespace cubeapp { namespace python {

	struct Interpreter::Impl
	{
		py::object main;
		py::dict globals;

		Impl()
			: main{}
			, globals{}
		{
			this->main = boost::python::import("__main__");
			this->globals = py::extract<py::dict>(this->main.attr("__dict__"));
		}
	};


	Interpreter::Interpreter()
		: _impl(new Impl)
	{ ETC_TRACE_CTOR(); }

	Interpreter::~Interpreter()
	{
		ETC_TRACE_DTOR();
		delete _impl;
		_impl = nullptr;
		if (Py_IsInitialized())
		{
			int i = 0;
			while (::PyGC_Collect() != 0 && i < 10)
				i++;
			Py_Finalize();
		}
	}

	boost::python::dict& Interpreter::globals()
	{ return _impl->globals; }

	void Interpreter::exec(std::string const& script)
	{
		ETC_TRACE.debug("Execute:\n", script);
		py::object res;
		cube::python::propagate_exception([&] {
			res = py::exec(script.c_str(),
			               _impl->globals,
			               _impl->globals);
		});
	}

	void Interpreter::setglobal(std::string const& key,
	                            std::string const& value)
	{
		ETC_LOG.debug("Set global string", key, "=", value);
		_impl->globals[key] = value;
	}

	void Interpreter::setglobal(std::string const& key,
	                            boost::python::object const& value)
	{
		ETC_LOG.debug("Set global", key, "=", value);
		_impl->globals[key] = value;
	}


	///////////////////////////////////////////////////////////////////////////
	// Initialization section

	static std::unique_ptr<Interpreter>& interpreter_ptr()
	{
		static std::unique_ptr<Interpreter> ptr;
		return ptr;
	}

	Interpreter& Interpreter::instance(boost::filesystem::path lib_dir)
	{
		if (interpreter_ptr() == nullptr)
		{
			// If python has been relocated, we want to use the new PYTHONHOME.
			std::string python_dir =
				"python" + std::to_string(PY_MAJOR_VERSION) + "." +
				std::to_string(PY_MINOR_VERSION);
			if (fs::is_directory(lib_dir / python_dir))
			{
				etc::sys::environ::set("PYTHONPATH", "");
				etc::sys::environ::set("PYTHONHOME", "");
				auto python_path = (lib_dir / python_dir).string();
#ifdef ETC_PLATFORM_WINDOWS
				etc::sys::environ::set(
					"PYTHONPATH",
					python_path +
					";" + python_path + "\\DLLs" +
					";" + python_path + "\\Lib"
				);
#else
				auto prefix = lib_dir.parent_path().string();
				etc::sys::environ::set("PYTHONHOME", prefix + ":" + prefix);
				etc::sys::environ::set("PYTHONPATH", python_path);
#endif
			}

			// If this variable is present, python might use the wrong
			// PYTHONHOME value.
			std::string old_path = etc::sys::environ::set("PATH", "");
			ETC_SCOPE_EXIT{
				etc::sys::environ::set("PATH", old_path);
			};

			ETC_LOG("Compiled with python", PY_VERSION);
			ETC_LOG("PYTHONPATH =", etc::sys::environ::get("PYTHONPATH", ""));
			ETC_LOG("PYTHONHOME =", etc::sys::environ::get("PYTHONHOME", ""));
			::Py_InitializeEx(0 /* No signal handlers */);
			ETC_LOG("Linked with python", ::Py_GetVersion());
			//wchar_t const* home = Py_GetPythonHome();
			//wchar_t const* path = Py_GetPath();
			//std::wcout << L"Python home:" << std::wstring{home ? home : L"nil"} << std::endl;
			//std::wcout << L"Python path:" << std::wstring{path ? path : L"nil"} << std::endl;
			interpreter_ptr().reset(new Interpreter);
			ETC_LOG.debug("Interpreter created successfuly");
		}

		ETC_ASSERT(interpreter_ptr() != nullptr);
		return *interpreter_ptr();
	}

	void Interpreter::release()
	{ interpreter_ptr().reset(); }

}} // !app::python
