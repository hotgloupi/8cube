#include "types.hpp"

#include "to_string.hpp"

#include <cxxabi.h>

namespace etc {

	bool
	demangle(std::string const& sym,
	         std::string& res,
	         std::string& error) throw()
	{
		size_t size;
		int status;
		char* demangled = abi::__cxa_demangle(sym.c_str(), 0, &size, &status);

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
	}

	std::string demangle(std::string const& sym)
	{
		std::string error;
		std::string res;
		if (!demangle(sym, res, error))
			throw std::runtime_error{
				etc::to_string("couldn't demangle", sym, ":", error)
			};
		return res;
	}

}
