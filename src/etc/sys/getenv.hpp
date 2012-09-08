#ifndef  ETC_SYS_GETENV_HPP
# define ETC_SYS_GETENV_HPP

# include "../exception.hpp"

namespace etc { namespace sys {

	/**
	 * @brief Get the associated value of @a key from the environment.
	 * @throws KeyError
	 */
	std::string getenv(std::string const& key);

	/**
	 * @brief   Get the associated value of @a key from the environment or
	 *          returns the @a default_value when not found.
	 */
	std::string getenv(std::string const& key,
	                   std::string const& value);

	ETC_EXCEPTION_CLASS(KeyError);
}}

#endif

