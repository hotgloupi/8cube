#include "environ.hpp"

#include <cstdlib>

namespace etc { namespace sys { namespace environ {

	std::string get(std::string const& key)
	{
		char const* val = ::getenv(key.c_str());
		if (val == nullptr)
			throw KeyError(key);
		return std::string{val};
	}

	std::string get(std::string const& key,
	                std::string const& default_value)
	{
		char const* val = ::getenv(key.c_str());
		if (val == nullptr)
			return default_value;
		return std::string{val};
	}

	bool contains(std::string const& key) noexcept
	{ return (::getenv(key.c_str()) != nullptr); }

	std::string set(std::string const& key,
	                std::string const& value)
	{
		std::string old = get(key, "");
#ifdef ETC_PLATFORM_WINDOWS
		if (::_putenv((key + "=" + value).c_str()) != 0)
#else
		if (::setenv(key.c_str(), value.c_str(), 1) < 0)
#endif
			throw std::runtime_error("Cannot set env var " + key);
		return old;
	}

	/**
	 * @brief Set a value into the environment if not already present and
	 * returns the final value.
	 */
	std::string set_default(std::string const& key,
	                        std::string const& value)
	{
		char const* old_value = ::getenv(key.c_str());
		if (old_value == nullptr)
		{
#ifdef ETC_PLATFORM_WINDOWS
			if (::_putenv((key + "=" + value).c_str()) != 0)
#else
			if (::setenv(key.c_str(), value.c_str(), 1) < 0)
#endif
				throw std::runtime_error("Cannot set env var " + key);
			return value;
		}
		return std::string{old_value};
	}

}}}
