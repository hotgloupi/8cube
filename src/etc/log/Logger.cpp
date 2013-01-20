#include "Logger.hpp"

#include <etc/meta/enum.hpp>
#include <etc/sys/getenv.hpp>

#include <boost/algorithm/string.hpp>

#include <cassert>
#include <vector>
#include <unordered_map>

#ifdef _WIN32
# include <Shlwapi.h>
# pragma comment(lib, "shlwapi.lib")
#else
# include <fnmatch.h>
#endif

namespace etc { namespace log {

	namespace {

		struct Pattern
		{
			enum Operation
			{
				add_match,
				remove_match,
			} op;
			std::string str;

			Pattern(Operation op, std::string str)
				: op(op)
				, str(str)
			{}
		};

		std::vector<Pattern> get_patterns()
		{
			std::string envvar = etc::sys::getenv("ETC_LOG_COMPONENTS", "");
			std::vector<std::string> patterns;
			boost::algorithm::split(
				patterns,
				envvar,
				boost::algorithm::is_any_of(","),
				boost::algorithm::token_compress_on
			);
			std::vector<Pattern> res;
			for (auto& pattern: patterns)
			{
				boost::algorithm::trim(pattern);
				if (pattern.size() == 0 ||
						(pattern.size() == 1 && (
							pattern[0] == '+' ||
							pattern[0] == '-')))
					continue;
				if (pattern[0] == '+')
					res.push_back(Pattern(Pattern::add_match, pattern.substr(1)));
				else if (pattern[0] == '-')
					res.push_back(Pattern(Pattern::remove_match, pattern.substr(1)));
				else
					res.push_back(Pattern(Pattern::add_match, pattern));
			}
			return res;
		}

		bool component_enabled(std::string const& name)
		{
			if (name.size() == 0)
				return true;
			static std::unordered_map<std::string, bool> components;
			auto it = components.find(name);
			if (it != components.end())
				return it->second;
			static std::vector<Pattern> patterns = get_patterns();
			bool enabled = false;
			for (auto const& pattern: patterns)
			{
#ifdef _WIN32
				if (::PathMatchSpec(name.c_str(), pattern.str.c_str()) == TRUE)
#else
				if (::fnmatch(pattern.str.c_str(), name.c_str(), 0) == 0)
#endif
					enabled = (pattern.op == Pattern::add_match);
			}
			return components[name] = enabled;
		}

		Level default_level()
		{
			std::string level_string = etc::sys::getenv("ETC_LOG_LEVEL", "INFO");
			std::unordered_map<std::string, Level> map{
				{"DEBUG", Level::debug},
				{"INFO", Level::info},
				{"WARN", Level::warn},
				{"WARNING", Level::warn},
				{"ERROR", Level::error},
				{"FATAL", Level::fatal},
			};
			auto it = map.find(level_string);
			if (it == map.end())
				return Level::info;
			return it->second;
		}

	} // !anonymous



	Logger& logger(std::string const& name)
	{
		static std::unordered_map<std::string, Logger*> loggers;
		static Level level = default_level();
		auto it = loggers.find(name);
		if (it != loggers.end())
			return *it->second;
		return *((loggers[name] = new Logger{name, level}));
	}

	Logger::Logger(std::string const& name,
	               Level level,
	               Flag const flags /* = Flag::level | Flag::component */)
		: _name(name)
		, _level(level)
		, _streams()
		, _flags{flags}
	{
		for (auto& stream : _streams)
			stream = OutStream(&std::cerr, false);
	}

	void Logger::_message(Line const& line,
	                      std::string const& message)
	{
		if (line.level < _level && !component_enabled(line.component))
			return;

		// every fields are strings.
		struct {
			std::string const   level;
			std::string const&  file;
			std::string const   line;
			std::string const&  function;
			std::string const&  component;
		} string = {
			etc::to_string(line.level),
			line.file,
			etc::to_string(line.line),
			line.function,
			line.component,
		};

		// Statically store max string size of each part.
		static struct {
			size_t level;
			size_t file;
			size_t line;
			size_t function;
			size_t component;
		} max_size = {0, 0, 0, 0, 0};

		// And update them.
#define _UPDATE_SIZE(__name)                                                  \
		if (max_size.__name < string.__name.size())                           \
			max_size.__name = string.__name.size()                            \
/**/

		_UPDATE_SIZE(level);
		_UPDATE_SIZE(file);
		_UPDATE_SIZE(line);
		_UPDATE_SIZE(function);
		_UPDATE_SIZE(component);
#undef _UPDATE_SIZE

		// Fetch the out stream.
		assert(meta::enum_<Level>::valid_index(line.level));
		unsigned int idx = meta::enum_<Level>::indexof(line.level);
		std::ostream* out = _streams[idx].out;
		assert(out != nullptr);

		switch (line.level)
		{
		case Level::warn:
			break;
			*out << "[33;01;33m";
		case Level::error:
			*out << "[33;03;31m";
			break;
		default:
			break;
		}
		// Print each part
#define _PRINT_PART(__name, __flag) \
		if (_flags & Flag::__flag) \
		{ \
			size_t delta = max_size.__name - string.__name.size(); \
			*out << '[' << std::string( \
						(delta / 2) + (delta % 2), \
						' ' \
					) \
				 << string.__name \
				 << std::string(delta / 2, ' ') << ']' \
			; \
		} \
/**/
		_PRINT_PART(level, level);
		_PRINT_PART(file, location);
		_PRINT_PART(line, location);
		_PRINT_PART(function, function);
		_PRINT_PART(component, component);
#undef _PRINT_PART

		*out <<  std::string(line.indent * 2, ' ') << message
		     << "[0m" << std::endl;
	}

}} // !etc::log
