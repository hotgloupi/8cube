#ifndef  ETC_SYS_ENVIRON_HPP
# define ETC_SYS_ENVIRON_HPP

# include <etc/api.hpp>
# include <etc/compiler.hpp>
# include <etc/exception.hpp>

# ifdef environ
#  undef environ
# endif

namespace etc { namespace sys { namespace environ {

	/**
	 * @brief Get the associated value of @a key from the environment.
	 * @throws KeyError
	 */
	ETC_API std::string get(std::string const& key);

	/**
	 * @brief   Get the associated value of @a key from the environment or
	 *          returns the @a default_value when not found.
	 */
	ETC_API std::string get(std::string const& key,
	                        std::string const& value);

	/**
	 * @brief Cast an environment variable to a type T.
	 */
	template<typename T>
	ETC_API T as(std::string const& key);

	template<typename T>
	T try_as(std::string const& key)
	{
		try { return as<T>(key); }
		catch (exception::ValueError const&) { return T{}; }
	}

	/**
	 * @brief Cast an environnement variable to a boolean.
	 *
	 * Returns true if an environment variable lower cased value is either "1",
	 * "yes", "ok", "on" or "true". It returns false if the variable is not
	 * defined, or when the value is either "0", "no", "ko", "off" or "false".
	 * In other cases, it throws a ValueError exception.
	 */
	template<>
	ETC_API bool as<bool>(std::string const& key);

	/**
	 * @brief Set a value into the environment and returns the old value or an
	 * empty string.
	 */
	ETC_API std::string set(std::string const& key,
	                        std::string const& value);

	/**
	 * @brief Set a value into the environment if not already present and
	 * returns the final value.
	 */
	ETC_API std::string set_default(std::string const& key,
	                                std::string const& value);

	/**
	 * @brief Returns whether or not a variable is in the environment.
	 */
	ETC_API bool contains(std::string const& key) ETC_NOEXCEPT;

}}}

#endif
