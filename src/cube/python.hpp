#ifndef CUBE_PYTHON_HPP
# define CUBE_PYTHON_HPP

// workaround a mingw bug
// http://lists.boost.org/boost-users/2010/12/65159.php
// workaround a bug on OS X 10.6
#  include <cmath>

# if (defined(__MINGW32__) || defined(mingw32) || defined(__MINGW64__) || defined(mingw64)) && (__GNUC__ == 4)


// workaround a mingw bug
// http://sourceforge.net/tracker/index.php?func=detail&aid=2373234&group_id=2435&atid=102435
#  include <cstddef> // std:size_t

// not needed in the last version of mingw
//int swprintf (wchar_t *, size_t, const wchar_t *, ...);

# endif

# include <etc/to_string.hpp>

// Workaround to remove silly macros defined somewhere by Python
# include <Python.h>
# undef toupper
# undef tolower
# undef isspace
# undef islower
# undef isupper
# undef isalpha
# undef isalnum


# include <memory>
# include <boost/shared_ptr.hpp>

namespace boost {

	template<class T>
	T* get_pointer(std::weak_ptr<T> const& ptr)
	{ return ptr.lock().get(); };

# ifdef BOOST_NO_CXX11_SMART_PTR
	template<typename T>
	T* get_pointer( std::shared_ptr<T> const& ptr)
	{ return ptr.get(); }
# endif
}

# include <boost/python.hpp>

namespace boost { namespace python {

	// Register std::unique_ptr as an HelderType
	template<class T>
	struct pointee<std::unique_ptr<T>> { typedef T type; };

	template<class T>
	struct pointee<std::weak_ptr<T>> { typedef T type; };

	inline
	std::string to_string(object o)
	{ return extract<std::string>(str(o)); }

	///////////////////////////////////////////////////////////////////////
	// Convert unique_ptr

	// This is involved when returning by value, and as copying is not
	// allowed for a unique_ptr, we can safely remove de const qualifiers.
	// This is meant to happen when returning unique_ptr<T>, for the
	// caller, the variable is moved.
	//
	// When boost python handle properly to move operation, this could go
	// away.

	template<typename T>
	struct unique_ptr_converter
	{
		static inline
		PyObject* convert(std::unique_ptr<T> const& value)
		{
			return boost::python::incref(
				boost::python::object(
					std::auto_ptr<T>(
						const_cast<std::unique_ptr<T>&>(value).release()
					)
				).ptr()
			);
		}
	};

	template<typename T>
	inline
	void register_unique_ptr_converter()
	{
		to_python_converter<std::unique_ptr<T>, unique_ptr_converter<T>>();
	}

	template<
		  typename C
		, typename Ret
		, typename Policy = return_value_policy<return_by_value>
	>
	inline
	object select_getter(Ret (C::*value)() const,
	                   Policy const& policy = Policy())
	{ return make_function(value, policy); }

	template<
		  typename C
		, typename Ret
		, typename Policy = return_internal_reference<>
	>
	inline
	object select_getter(Ret const& (C::*value)() const,
	                   Policy const& policy = Policy())
	{ return make_function(value, policy); }

	template<
		  typename Ret
		, typename C
		, typename Arg
		, typename Policy = return_self<>
	>
	inline
	object select_setter(Ret (C::*value)(Arg),
	                   Policy const& policy = Policy())
	{ return make_function(value, policy); }

	template<typename T>
	std::string stringof(T const& el)
	{ return etc::to_string(el); }
}}

# include <cube/exception.hpp>
# include <etc/backtrace.hpp>

#define CUBE_PYTHON_DOCSTRING_OPTIONS()                                       \
	boost::python::docstring_options __docstring_options{true, true, false};  \
/**/

namespace cube { namespace python {

	template<typename Fn, typename... Args>
	void propagate_exception(Fn&& fn, Args&&... args)
	{
		try { fn(std::forward<Args>(args)...); }
		catch (boost::python::error_already_set const&)
		{
			using namespace boost::python;
			object exception, value, traceback;
			{
				PyObject* py_exception;
				PyObject* py_value;
				PyObject* py_traceback;
				PyErr_Fetch(&py_exception, &py_value, &py_traceback);
				//PyErr_Display(py_exception, py_value, py_traceback);

				if (py_exception == nullptr)
					throw std::runtime_error(
						"Internal boost python error while catching a python "
						"exception"
					);
				exception = object(handle<>(py_exception));
				value = object(handle<>(allow_null(py_value)));
				traceback = object(handle<>(allow_null(py_traceback)));
			}

			auto msg = to_string(exception.attr("__name__"))
				+ ": " + to_string(value);
			etc::backtrace::Backtrace bt;
			if (traceback.ptr() != nullptr)
			{
				object tb_mod = import("traceback");
				list tb = extract<list>(tb_mod.attr("extract_tb")(traceback));
				for (int i = 0, l = len(tb); i < l; ++i)
				{
					tuple el = extract<tuple>(tb[i]);
					std::string file = to_string(el[0]);
					auto pos = file.find("/cube");
					if (pos != std::string::npos)
						file = file.substr(pos + 1, std::string::npos);
					bt.insert(
						bt.begin(), //XXX painfully slow
						etc::backtrace::StackFrame{
							file + ": " + to_string(el[3]),
							extract<unsigned int>(el[1]),
							to_string(el[2]),
							to_string(el[2]),
							nullptr,
							0
						}
					);
				}
			}
			throw cube::exception::Exception(msg, &bt);
		}
	}
}}

#endif
