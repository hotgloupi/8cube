#ifndef  ETC_LOG_LINE_HPP
# define ETC_LOG_LINE_HPP

# include "Level.hpp"

# include <etc/types.hpp>

# include <string>

namespace etc { namespace log {

	struct Line
	{
	public:
		Level           level;
		std::string     file;
		etc::size_type  line;
		std::string     function;
		std::string     component;
		etc::size_type  indent;

	public:
		Line(Level level,
		     std::string const& file,
		     etc::size_type line,
		     std::string const& function,
		     std::string const& component,
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
			, file{std::move(o.file)}
			, line{o.line}
			, function{std::move(o.function)}
			, component{std::move(o.component)}
			, indent{o.indent}
		{}

	};

}}

#endif

