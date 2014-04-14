#include "default_directory.hpp"

#include <etc/sys/environ.hpp>

#include <wrappers/windows.hpp>

namespace etc { namespace temp {

	std::string default_directory()
	{
#ifdef _WIN32
		TCHAR buf [MAX_PATH];

		if (GetTempPath (MAX_PATH, buf) == 0)
			return "C:\\";
		return std::string(buf);
#else
		return sys::environ::get("TMPDIR", "/tmp");
#endif
	}


}}
