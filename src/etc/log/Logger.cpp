
#include <cassert>

#include "Logger.hpp"

namespace etc { namespace log {

	Logger::Logger(Logger::Level level)
		: _level(level)
		, _streams{
			{&std::cerr, false},
			{&std::cerr, false},
			{&std::cerr, false},
			{&std::cerr, false},
			{&std::cerr, false},
		}
	{}

	void Logger::_message(Level level, std::string const& message)
	{
		static size_t const max_level_idx =
			static_cast<size_t>(Logger::Level::_maxvalue);

		static std::string const level_strings[max_level_idx] = {
			"DEBUG",
			"INFO",
			"WARNING",
			"ERROR",
			"FATAL",
		};
		size_t idx = static_cast<size_t>(level);
		assert(idx < max_level_idx);
		std::ostream* out = this->_streams[idx].out;
		assert(out != nullptr);
		(*out) << '[' << level_strings[idx] << "] " << message << std::endl; // XXX
	}

}} // !etc::log
