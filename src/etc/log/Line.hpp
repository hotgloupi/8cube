#ifndef  ETC_LOG_LINE_HPP
# define ETC_LOG_LINE_HPP

# include "Level.hpp"

# include <string>

namespace etc { namespace log {

	struct Line
	{
	public:
		Level           level;
		std::string     file;
		unsigned int    line;
		std::string     function;
		std::string     component;
		unsigned int    indent;

	public:
		Line(Level level,
		     std::string const& file,
		     unsigned int line,
		     std::string const& function,
		     std::string const& component,
			 unsigned int indent)
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

