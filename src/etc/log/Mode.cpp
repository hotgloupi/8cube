#include "Mode.hpp"

#include <etc/sys/environ.hpp>

#include <iostream>

namespace etc { namespace log {

	Mode default_mode() ETC_NOEXCEPT
	{
		bool async = true;
		if (sys::environ::contains("ETC_LOG_ASYNC"))
			async = sys::environ::try_as<bool>("ETC_LOG_ASYNC");
		else if (sys::environ::contains("ETC_LOG_SYNC"))
			async = !sys::environ::try_as<bool>("ETC_LOG_SYNC");
		return (async ? Mode::asynchroneous : Mode::synchroneous);
	}

	std::ostream& operator <<(std::ostream& out, Mode const mode)
	{
		switch (mode)
		{
		case Mode::asynchroneous:
			return out << "Mode::asynchroneous";
		case Mode::synchroneous:
			return out << "Mode::synchroneous";
		default:
			return out << "Mode::INVALID";
		}
	}

}}
