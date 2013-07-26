#ifndef  WRAPPERS_PYTHON_BOOST_HPP
# define WRAPPERS_PYTHON_BOOST_HPP

/**
 * This Macro can be used to link statically with python. Unfortunatly,
 * it's not working because boost::python defines a global registry
 * that has to be shared between all python modules ...
 * Tested on windows 7.
 */
//#define BOOST_PYTHON_STATIC_LIB 1

# if (defined(__MINGW32__) || defined(mingw32) || defined(__MINGW64__) || defined(mingw64)) && (__GNUC__ == 4)

// workaround a mingw bug
// http://lists.boost.org/boost-users/2010/12/65159.php
#  include <cmath>

// workaround a mingw bug
// http://sourceforge.net/tracker/index.php?func=detail&aid=2373234&group_id=2435&atid=102435
#  include <cstddef> // std:size_t
int swprintf (wchar_t *, size_t, const wchar_t *, ...);

# endif

// Workaround to remove silly macros defined somewhere by Python
# include <Python.h>
# undef toupper
# undef tolower
# undef isspace
# undef islower
# undef isupper
# undef isalpha
# undef isalnum

# include <boost/python.hpp>

# include <memory>

namespace boost { namespace python {

	// Register std::unique_ptr as an HelderType
	template<class T>
	struct pointee<std::unique_ptr<T>>
	{
		typedef T type;
	};

	template<typename T>
	std::string stringof(T const& val)
	{
		std::stringstream ss;
		ss << val;
		return ss.str();
	};
}}

#define BOOST_PYTHON_DOCSTRING_OPTIONS()                                      \
	boost::python::docstring_options __docstring_options{true, true, false};  \
/**/

#endif
