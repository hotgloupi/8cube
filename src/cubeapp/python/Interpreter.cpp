#include <wrappers/boost/python.hpp>
#include <boost/python/import.hpp>
#include <boost/algorithm/string.hpp>

#include "Interpreter.hpp"
#ifdef environ
# undef environ
#endif
#include <etc/log.hpp>
#include <etc/platform.hpp>
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
	{}

	Interpreter::~Interpreter()
	{
		delete _impl;
		_impl = nullptr;
	}

	bool Interpreter::exec(std::string const& script)
	{
		try {
			py::exec(script.c_str(),
			         _impl->main_namespace,
			         _impl->main_namespace);
			return true;
		} catch (boost::python::error_already_set const&) {
			//std::cerr << "Python exception:\n";
			PyErr_Print();
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
/*
	static
	bool contains_libpython(fs::path lib_dir, std::string dirname)
	{
		fs::directory_iterator dir(lib_dir), end;
		for (; dir != end; ++dir)
		{
			fs::path p(*dir);
			if (boost::algorithm::starts_with(p.filename().string(), filename))
				return true;
		}
		return false;
	}*/

	Interpreter& Interpreter::instance(boost::filesystem::path lib_dir)
	{
		static Interpreter* _interpreter = nullptr;

		if (_interpreter == nullptr)
		{
			// If python has been relocated, we want to use the new PYTHONHOME.
			std::string python_dir =
				"python" + std::to_string(PY_MAJOR_VERSION) + "." +
				std::to_string(PY_MINOR_VERSION);
			if (fs::is_directory(lib_dir / python_dir))
			{
				etc::sys::environ::set("PYTHONPATH", "");
				etc::sys::environ::set("PYTHONHOME", "");
#ifdef ETC_PLATFORM_WINDOWS
				auto python_path = (lib_dir / python_dir).string();
				etc::sys::environ::set(
					"PYTHONPATH",
					python_path +
					";" + python_path + "\\DLLs" +
					";" + python_path + "\\Lib"
				);
#else
				auto prefix = lib_dir.parent_path().string();
				etc::sys::environ::set("PYTHONHOME", prefix + ":" + prefix);
#endif
				ETC_LOG("PYTHONPATH =", etc::sys::environ::get("PYTHONPATH", ""));
				ETC_LOG("PYTHONHOME =", etc::sys::environ::get("PYTHONHOME", ""));
			}

			// If this variable is present, python might use the wrong
			// PYTHONHOME value.
			std::string old_path = etc::sys::environ::set("PATH", "");

			ETC_LOG("Compiled with python", PY_VERSION);
			::Py_Initialize();
			ETC_LOG("Linked with python", ::Py_GetVersion());
			_interpreter = new Interpreter;

			etc::sys::environ::set("PATH", old_path);
		}
		assert(_interpreter != nullptr);

		return *_interpreter;
	}

}} // !app::python
