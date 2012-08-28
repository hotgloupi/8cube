#ifndef  ETC_LOG_LEVEL_HPP
# define ETC_LOG_LEVEL_HPP

# include <iosfwd>

namespace etc { namespace log {

	enum class Level
	{
		debug = 0,
		info,
		warn,
		error,
		fatal,

		_maxvalue,
	};

	std::ostream& operator <<(std::ostream& out, Level level);

}}

#endif

