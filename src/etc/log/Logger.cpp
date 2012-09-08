#include "Logger.hpp"

#include <etc/meta/enum.hpp>

#include <cassert>
#include <unordered_map>

namespace etc { namespace log {

    Logger& logger(std::string const& name)
	{
		static std::unordered_map<std::string, Logger*> loggers;
		auto it = loggers.find(name);
		if (it != loggers.end())
			return *it->second;
		return *((loggers[name] = new Logger{name, Level::info}));
	}

	Logger::Logger(std::string const& name,
	               Level level,
	               Flag const flags /* = Flag::level | Flag::component */)
		: _name(name)
		, _level(level)
		, _streams{}
		, _flags{flags}
	{
		for (auto& stream : _streams)
			stream = { &std::cerr, false };
	}

	void Logger::_message(Line const& line,
	                      std::string const& message)
	{
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

		*out <<  std::string(line.indent * 2, ' ') << message << std::endl;
	}

}} // !etc::log
