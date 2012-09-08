#include "getenv.hpp"

#include <cstdlib>

namespace etc { namespace sys {

	std::string getenv(std::string const& key)
	{
		char const* val = ::getenv(key.c_str());
		if (val == nullptr)
			throw KeyError("KeyError: '" + key + "'");
		return std::string{val};
	}

	std::string getenv(std::string const& key,
	                   std::string const& default_value)
	{
		char const* val = ::getenv(key.c_str());
		if (val != nullptr)
			return std::string{val};
		else
			return default_value;
	}

}}
