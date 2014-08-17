#include "Console.hpp"

#include <boost/algorithm/string.hpp>

#include <vector>
#include <iostream>

namespace etc { namespace log { namespace backend {

	struct Console::Impl
	{

	};

	Console::Console()
		: _this{new Impl}
	{}

	Console::~Console()
	{}

	void Console::_send_line(Line line, std::string message) ETC_NOEXCEPT
	{
		static char const* level_strings[] = {
			"DEBUG",
			"INFO",
			"WARN",
			"ERROR",
			"FATAL",
		};
		char const* level_string = level_strings[(size_t)line.level];
		std::string line_string{std::to_string(line.line)};

		struct Size {
			size_t level;
			size_t file;
			size_t line;
			size_t function;
			size_t component;
		};

		static Size max_size{8, 0, 4, 0, 50};

		Size size{
			strlen(level_string),
			strlen(line.file),
			line_string.size(),
			strlen(line.function),
			line.component.size(),
		};


		// And update them.
#define _UPDATE_SIZE(__name)                                                  \
		if (max_size.__name < size.__name)                                \
			max_size.__name = size.__name;                                \
/**/
		_UPDATE_SIZE(line);
		_UPDATE_SIZE(file);
		_UPDATE_SIZE(function);
		_UPDATE_SIZE(component);
#undef _UPDATE_SIZE

#if 0 // doesn't work: exit handlers erase the string while somebody logs at exit
		static boost::thread_specific_ptr<std::string> result_storage;
		if (result_storage.get() == nullptr)
			result_storage.reset(new std::string{});
		std::string& res = *result_storage;
		res.clear();
#else
		// XXX some kind of optimisation needed ?
		std::string res;
#endif

#ifdef _WIN32
		static HANDLE console_handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
# define DARK_BLUE    1
# define DARK_GREEN   2
# define DARK_CYAN    3
# define DARK_RED     4
# define DARK_PINK    5
# define DARK_YELLOW  6
# define WHITE        7
# define GRAY         8
# define BLUE         9
# define GREEN        10
# define CYAN         11
# define RED          12
# define PINK         13
# define YELLOW       14
# define BOLD_WHITE   15
		int color = WHITE;
		switch (line.level)
		{
		case Level::info:
			color = BOLD_WHITE;
			break;
		case Level::warn:
			color = YELLOW;
			break;
		case Level::error:
			color = DARK_RED;
			break;
		case Level::fatal:
			color = DARK_PINK;
			break;
		default:
			break;
		}
		if (color != WHITE)
			::SetConsoleTextAttribute(console_handle, color);
#else
		static std::string color;
		switch (line.level)
		{
		case Level::info:
			color = "[1m";
			break;
		case Level::warn:
			color = "[33;01;33m";
			break;
		case Level::error:
			color = "[33;01;91m";
			break;
		case Level::fatal:
			color = "[33;01;95m";
			break;
		default:
			color.clear();
			break;
		}
#endif
		// Print each part
#define _PRINT_PART(__name, __var, __flag) \
		{ \
			size_t delta = max_size.__name - size.__name; \
			res.append(1, '['); \
			res.append( \
				(delta / 2) + (delta % 2), \
				' ' \
			); \
			res.append(__var); \
			res.append(delta / 2, ' '); \
			res.append(1, ']'); \
		} \
/**/
		_PRINT_PART(level, level_string, level);
		//_PRINT_PART(file, line.file, location);
		//_PRINT_PART(line, line_string, location);
		//_PRINT_PART(function, line.function, function);
		_PRINT_PART(component, line.component, component);
#undef _PRINT_PART

		res.append(line.indent * 2, ' ');

		std::string trimmed_message = message;
		boost::algorithm::trim_if(
			trimmed_message,
			boost::algorithm::is_any_of(" \t\n\r"));
		std::vector<std::string> lines;
		boost::split(lines,
					 trimmed_message,
					 boost::algorithm::is_any_of("\r\n"),
					 boost::token_compress_on);
		if (lines.size() == 1)
		{
			res.append(lines[0]);
		}
		else if (lines.size() > 1)
		{
			std::string indent(res.size() - line.indent * 2, '.');
			indent.append(line.indent * 2, ' ');
			res.append(lines[0]);
			for (size_t i = 1; i < lines.size(); i++)
			{
				res.append("\n");
				res.append(indent);
				res.append(lines[i]);
			}
		}
		res.append("\n");

#ifndef _WIN32
		std::cerr << color << res << "[0m";
#else
		std::cerr << res;
		if (color != WHITE)
			::SetConsoleTextAttribute(console_handle, WHITE);
#endif
	}

}}}
