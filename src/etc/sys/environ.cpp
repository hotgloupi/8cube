#include "environ.hpp"

#include <cstdlib>

namespace etc { namespace sys { namespace environ {

	std::string get(std::string const& key)
	{
#ifdef _WIN32
		char val[512];
		size_t size;
		if (::getenv_s(&size, val, key.c_str()) != 0)
			throw exception::Exception(
				"Cannot fetch env variable '" + key + "'"
			);
		if (size == 0)
#else
		char const* val = ::getenv(key.c_str());
		if (val == nullptr)
#endif
			throw KeyError("KeyError: '" + key + "'");
		return std::string(val);
	}

	std::string get(std::string const& key,
	                std::string const& default_value)
	{
#ifdef _WIN32
		char val[512];
		size_t size;
		if (::getenv_s(&size, val, key.c_str()) != 0)
			throw exception::Exception(
				"Cannot fetch env variable '" + key + "'"
			);
		if (size == 0)
#else
		char const* val = ::getenv(key.c_str());
		if (val == nullptr)
#endif
			return default_value;
		else
			return std::string(val);
	}

	std::string set(std::string const& key,
	                std::string const& value)
	{
		std::string old = get(key, "");
		::setenv(key.c_str(), value.c_str(), 1);
		return old;
	}

	/**
	 * @brief Set a value into the environment if not already present and
	 * returns the final value.
	 */
	std::string set_default(std::string const& key,
	                        std::string const& value)
	{
		::setenv(key.c_str(), value.c_str(), 0);
		return get(key);
	}

}}}
