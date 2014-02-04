#include "environ.hpp"

#include <boost/algorithm/string.hpp>

#include <cstdlib>

namespace etc { namespace sys { namespace environ {

	using exception::Exception;
	using exception::KeyError;
	using exception::ValueError;

	static
	char const* raw_get(std::string const& key)
	{ return ::getenv(key.c_str()); }

	std::string get(std::string const& key)
	{
		char const* val = raw_get(key);
		if (val == nullptr)
			throw KeyError(key);
		return std::string{val};
	}

	std::string get(std::string const& key,
	                std::string const& default_value)
	{
		char const* val = raw_get(key);
		if (val == nullptr)
			return default_value;
		return std::string{val};
	}

	template<>
	ETC_API bool as<bool>(std::string const& key)
	{
		char const* val = raw_get(key);
		if (val == nullptr)
			return false;
		std::string value{val};
		boost::to_lower(value);
		static std::string true_values[] = {"1", "yes", "ok", "on", "true"};
		static std::string false_values[] = {"0", "no", "ko", "off", "false"};
		for (auto const& s: true_values)
			if (s == value) return true;
		for (auto const& s: false_values)
			if (s == value) return false;
		throw ValueError{"Cannot cast '" + value + "' to bool"};
	}

	bool contains(std::string const& key) ETC_NOEXCEPT
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
			throw Exception("Cannot set env var " + key);
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
				throw Exception("Cannot set env var " + key);
			return value;
		}
		return std::string{old_value};
	}

}}}
