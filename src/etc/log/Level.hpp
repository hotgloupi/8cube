#ifndef  ETC_LOG_LEVEL_HPP
# define ETC_LOG_LEVEL_HPP

# include "../api.hpp"

# include <iosfwd>

namespace etc { namespace log {

	enum class ETC_API Level
	{
		debug = 0,
		info,
		warn,
		error,
		fatal,

		_maxvalue,
	};

	ETC_API std::ostream& operator <<(std::ostream& out, Level level);

}}

#endif

