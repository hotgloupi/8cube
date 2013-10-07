#ifndef  ETC_LOG_LINE_HPP
# define ETC_LOG_LINE_HPP

# include "Level.hpp"

# include <etc/types.hpp>

# include <string>
# include <utility>

namespace etc { namespace log {

	struct Line
	{
	public:
		Level           level;
		char const*     file;
		etc::size_type  line;
		char const*     function;
		char const*     component;
		etc::size_type  indent;

	public:
		Line(Level level,
		     char const* file,
		     etc::size_type line,
		     char const* function,
		     char const* component,
			 etc::size_type indent)
			: level{level}
			, file{file}
			, line{line}
			, function{function}
			, component{component}
			, indent(indent)
		{}

		Line(Line const& o)
			: level{o.level}
			, file{o.file}
			, line{o.line}
			, function{o.function}
			, component{o.component}
			, indent{o.indent}
		{}

		Line(Line&& o)
			: level{o.level}
			, file(std::move(o.file))
			, line{o.line}
			, function(std::move(o.function))
			, component(std::move(o.component))
			, indent{o.indent}
		{}

	};

}}

#endif

