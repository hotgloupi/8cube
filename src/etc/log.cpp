#include "log.hpp"

namespace etc { namespace log {

    Logger& default_logger()
	{
		static Logger* logger = new Logger{Logger::Level::debug};
		return *logger;
	}

}}
