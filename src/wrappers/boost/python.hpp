#ifndef  _PYTHON_BOOST_HPP
# define _PYTHON_BOOST_HPP

#define BOOST_PYTHON_STATIC_LIB 1

# if (defined(__MINGW32__) || defined(mingw32) || defined(__MINGW64__) || defined(mingw64)) && (__GNUC__ == 4)

// workaround a mingw bug
// http://lists.boost.org/boost-users/2010/12/65159.php
#  include <cmath>

// workaround a mingw bug
// http://sourceforge.net/tracker/index.php?func=detail&aid=2373234&group_id=2435&atid=102435
#  include <cstddef> // std:size_t
int swprintf (wchar_t *, size_t, const wchar_t *, ...);

# endif

# include <boost/python.hpp>

namespace boost { namespace python {

	// Register std::unique_ptr as an HelderType
	template<class T>
	struct pointee<std::unique_ptr<T>>
	{
		typedef T type;
	};

}}

#endif
