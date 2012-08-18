#include "log.hpp"

#include <unordered_map>

namespace etc { namespace log {

    Logger& logger(std::string const& name)
	{
		static std::unordered_map<std::string, Logger*> loggers;
		auto it = loggers.find(name);
		if (it != loggers.end())
			return *it->second;
		return *((loggers[name] = new Logger{name}));
	}

}}
