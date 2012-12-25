#include "constants.hpp"

#include "exception.hpp"

#include <etc/path.hpp>
#include <etc/platform.hpp>
#include <etc/sys/getenv.hpp>

#ifdef ETC_WINDOWS
# include <wrappers/windows.hpp>
#else
# include <unistd.h>        /* getuid() */
# include <sys/types.h>
# include <pwd.h>           /* getpwuid */
#endif

namespace {

	std::string _home_directory()
	{
#ifdef ETC_WINDOWS
		TCHAR path[MAX_PATH];
		auto res = SHGetFolderPath(
			nullptr,
			CSIDL_PERSONAL | CSIDL_FLAG_CREATE,
			nullptr,
			0,
			path
		);
		if (res == S_OK)
			return std::string{path};
		else
			return etc::sys::getenv("HOMEPATH");
#else
		struct passwd* pw = ::getpwuid(getuid());
		if (pw != nullptr && pw->pw_dir != nullptr)
			return std::string{pw->pw_dir};
		else
			return etc::sys::getenv("HOME", "/tmp");
#endif
	}


	std::string _config_directory()
	{
#ifdef ETC_WINDOWS
# error "Not implemented"
#else
		return etc::path::join(
			_home_directory(),
			etc::sys::getenv(
				"CUBE_CONFIG_DIRECTORY_NAME",
				".config"
			)
		);
#endif
	}

} //!anonymous

namespace cube { namespace constants {

	typedef exception::Exception Exception;

	namespace system {

		std::string const& home_directory()
		{
			static std::string const dir = _home_directory();
			return dir;
		}

		std::string const& config_directory()
		{
			static std::string const dir = _config_directory();
			return dir;
		}

	} //!system

	namespace application {

		std::string const& name(std::string const& name)
		{
			static std::string app_name;
			if (name.size())
			{
				if (app_name.size())
					throw Exception{
						"The application name is already set"
					};
				app_name = name;
			}
			if (!app_name.size())
				throw Exception{"No application name has been set"};
			return app_name;
		}

		std::string const& config_directory()
		{
			static std::string const dir = etc::path::join(
				system::config_directory(),
				name()
			);
			return dir;
		}

	} //!application

}} //!cube::constants

