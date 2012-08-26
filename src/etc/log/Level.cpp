#include "Level.hpp"

#include <ostream>

namespace etc { namespace log {

	std::ostream& operator <<(std::ostream& out, Level level)
	{
		static char const* strs[] = {
			"DEBUG",
			"INFO",
			"WARNING",
			"ERROR",
			"FATAL",
		};
		return (out << strs[(unsigned int) level]);
	}

}}
