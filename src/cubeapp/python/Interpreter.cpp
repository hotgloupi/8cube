#include <wrappers/boost/python.hpp>
#include <boost/python/import.hpp>

#include "Interpreter.hpp"

#include <wrappers/boost/filesystem.hpp>

#include <cassert>
#include <iostream>
#include <stdexcept>

namespace py = boost::python;
namespace fs = boost::filesystem;

namespace cubeapp { namespace python {

	struct Interpreter::Impl
	{
		py::object main_module;
		py::object main_namespace;

		Impl()
			: main_module(boost::python::import("__main__"))
			, main_namespace{main_module.attr("__dict__")}
		{}
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
			std::cerr << "Python exception:\n";
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

	Interpreter& Interpreter::instance()
	{
		static Interpreter* _interpreter = nullptr;

		if (_interpreter == nullptr)
		{
			::Py_Initialize();
			_interpreter = new Interpreter;
		}
		assert(_interpreter != nullptr);
		return *_interpreter;

	}

}} // !app::python
