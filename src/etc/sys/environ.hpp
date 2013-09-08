#ifndef  ETC_SYS_ENVIRON_HPP
# define ETC_SYS_ENVIRON_HPP

# include "../api.hpp"
# include "../exception.hpp"

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
	 * Exception thrown when the key is not found.
	 */
	struct ETC_API_EXCEPTION KeyError
		: public etc::exception::Exception
	{
		KeyError(std::string const& msg)
			: etc::exception::Exception(msg)
		{}
	};

}}}

#endif
