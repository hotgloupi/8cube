#include <wrappers/boost/python.hpp>
#include <boost/python/import.hpp>
#include <boost/algorithm/string.hpp>

#include "Interpreter.hpp"
#ifdef environ
# undef environ
#endif
#include <etc/log.hpp>
#include <etc/platform.hpp>
#include <etc/scope_exit.hpp>
#include <etc/sys/environ.hpp>

#include <cassert>
#include <iostream>
#include <stdexcept>

namespace py = boost::python;
namespace fs = boost::filesystem;

ETC_LOG_COMPONENT("cubeapp.python.Interpreter");

namespace cubeapp { namespace python {

	struct Interpreter::Impl
	{
		py::object main_module;
		py::object main_namespace;

		Impl()
			: main_module{}
			, main_namespace{}
		{
			this->main_module = boost::python::import("__main__");
			this->main_namespace = main_module.attr("__dict__");
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
			Py_Finalize();
	}

	bool Interpreter::exec(std::string const& script)
	{
		try {
			py::exec(script.c_str(),
			         _impl->main_namespace,
			         _impl->main_namespace);
			return true;
		} catch (boost::python::error_already_set const&) {
			PyObject* type;
			PyObject* value;
			PyObject* traceback;
			PyErr_Fetch(&type, &value, &traceback);
			if (type == nullptr)
			{
				ETC_LOG.fatal("Internal boost python error");
			}
			else if (PyErr_GivenExceptionMatches(type, PyExc_SystemExit))
			{
				ETC_LOG.error("Exited early (return value is 1)");
			}
			else
			{
				ETC_LOG.fatal("Python main exited with errors");
				PyErr_Display(type, value, traceback);
				PyErr_Restore(type, value, traceback);
			}
		} catch (...) {
			std::cerr << "This is fucking unbelievable\n";
		}

		return false;
	}

	void Interpreter::setglobal(std::string const& key,
	                            std::string const& value)
	{
		py::dict d = py::extract<py::dict>(_impl->main_namespace);
		d[key] = value;
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
			::Py_Initialize();
			ETC_LOG("Linked with python", ::Py_GetVersion());
			//std::wcout << L"Python home:" << std::wstring{Py_GetPythonHome() || L"nil"};
			//std::wcout << L"Python path:" << std::wstring{Py_GetPath() || L"nil"};
			interpreter_ptr().reset(new Interpreter);

		}

		assert(interpreter_ptr() != nullptr);
		return *interpreter_ptr();
	}

	void Interpreter::release()
	{ interpreter_ptr().reset(); }

}} // !app::python
