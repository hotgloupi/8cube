#ifndef  CUBE_CONSTANTS_HPP
# define CUBE_CONSTANTS_HPP

# include <cube/api.hpp>

# include <string>

namespace cube { namespace constants {

	namespace system {

		/// Retrieve the current user home directory
		CUBE_API
		std::string const& home_directory();

		/**
		 * @brief The default configuration directory for applications.
		 *
		 * This function will use the environment variable
		 * `CUBE_CONFIG_DIRECTORY_NAME` if available.
		 */
		CUBE_API
		std::string const& config_directory();

	}

	namespace application {

		/**
		 * @brief Set or get the application name
		 *
		 * Some constants rely on the fact that the current application has a
		 * name. You should set it as soon as you can when launching the
		 * application. Once set, you cannot change the application name.
		 *
		 * @return the application name.
		 *
		 * @throw When you get the name while it has not been set, or if you
		 *        try to set it twice.
		 */
		CUBE_API
		std::string const& name(std::string const& name = "");

		/**
		 * @brief The application configuration default directory.
		 *
		 * @note Rely on `system::config_directory()` and `application::name()`.
		 */
		CUBE_API
		std::string const& config_directory();

	}

}}

#endif

