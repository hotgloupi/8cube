#ifndef ETC_LOG_DETAIL_PATTERN_HPP
# define ETC_LOG_DETAIL_PATTERN_HPP

# include <etc/log/Level.hpp>

# include <string>
# include <vector>

namespace etc { namespace log { namespace detail {

	struct Pattern
	{
		enum Operation
		{
			add_match = 1,
			remove_match = -1,
		} op;
		std::string str;
		Level level;

		Pattern(Operation op, std::string str, Level level)
			: op(op)
			, str(str)
			, level{level}
		{}

	};

	std::vector<Pattern> get_patterns(std::string const& str);

}}}

#endif
