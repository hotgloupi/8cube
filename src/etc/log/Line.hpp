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
		Level              level;
		char const*        file;
		etc::size_type     line;
		char const*        function;
		std::string const& component;
		etc::size_type     indent;

	public:
		Line() = delete;
		Line(Level const level,
		     char const* file,
		     etc::size_type const line,
		     char const* function,
		     std::string const& component,
			 etc::size_type indent) ETC_NOEXCEPT
			: level{level}
			, file{file}
			, line{line}
			, function{function}
			, component(component)
			, indent(indent)
		{}

		Line(Line const& o) ETC_NOEXCEPT
			: level{o.level}
			, file{o.file}
			, line{o.line}
			, function{o.function}
			, component(o.component)
			, indent{o.indent}
		{}
	};

}}

#endif

