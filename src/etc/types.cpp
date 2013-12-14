#include "types.hpp"

#include "to_string.hpp"

#include <boost/config.hpp>

#ifndef BOOST_MSVC
# include <cxxabi.h>
#endif

#include <iostream>

namespace etc {

	bool
	demangle(std::string const& sym,
	         std::string& res,
	         std::string& error) ETC_NOEXCEPT
	{
#ifdef BOOST_MSVC
		error = "Not implemented";
		return false;
#else
		size_t size;
		int status;
		char* demangled;
# ifdef _WIN32
		if (sym.size() && sym[0] == 'Z')
			demangled = abi::__cxa_demangle(("_" + sym).c_str(), 0, &size, &status);
		else
			demangled = abi::__cxa_demangle(sym.c_str(), 0, &size, &status);
# else
		demangled = abi::__cxa_demangle(sym.c_str(), 0, &size, &status);
# endif

		switch (status)
		{
		case 0:
			{
				res = demangled;
				::free(demangled);
				return true;
			}
		case -1:
			error = "memory allocation failure";
			return false;
		case -2:
			error = "not a valid name under the C++ ABI mangling rules";
			return false;
		case -3:
			error = "invalid argument";
			return false;
		default:
			std::abort();
		}
#endif
		return false;
	}

	template<throw_policy tp>
	ETC_API
	std::string demangle(std::string const& sym)
		ETC_NOEXCEPT_IF(tp == no_throw_policy)
	{
		std::string error;
		std::string res;
		if (!demangle(sym, res, error))
			if (tp == can_throw_policy)
				throw std::runtime_error(
					etc::to_string("couldn't demangle", sym, ":", error)
				);
			else
				return sym;
		return res;
	}

	template
	ETC_API
	std::string demangle<can_throw_policy>(std::string const& sym)
		ETC_NOEXCEPT_IF(false);

	template
	ETC_API
	std::string demangle<no_throw_policy>(std::string const& sym)
		ETC_NOEXCEPT_IF(true);

}
