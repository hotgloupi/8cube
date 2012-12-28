#ifndef  ETC_SYS_GETENV_HPP
# define ETC_SYS_GETENV_HPP

# include "../api.hpp"
# include "../exception.hpp"

namespace etc { namespace sys {

	/**
	 * @brief Get the associated value of @a key from the environment.
	 * @throws KeyError
	 */
	ETC_API std::string getenv(std::string const& key);

	/**
	 * @brief   Get the associated value of @a key from the environment or
	 *          returns the @a default_value when not found.
	 */
	ETC_API std::string getenv(std::string const& key,
	                           std::string const& value);

	struct ETC_API_EXCEPTION KeyError
		: public etc::exception::Exception
	{
		KeyError(std::string const& msg)
			: etc::exception::Exception(msg)
		{}
	};
}}

#endif

