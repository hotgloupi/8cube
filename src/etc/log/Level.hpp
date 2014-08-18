#ifndef  ETC_LOG_LEVEL_HPP
# define ETC_LOG_LEVEL_HPP

# include <cube/api.hpp>

# include <string>
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

	ETC_API std::ostream& operator <<(std::ostream& out, Level level);

	Level level_from_string(std::string const& str,
	                        bool ignore_case = true);

	Level default_level();

}}

#endif
