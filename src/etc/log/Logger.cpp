
#include <cassert>

#include "Logger.hpp"

namespace etc { namespace log {

	Logger::Logger(std::string const& name, Logger::Level level)
		: _name(name)
		, _level(level)
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
		std::string msg = "[" + level_strings[idx] + "] ";
		if (_name.size())
			msg += _name + ": ";
		*out << (msg + message + "\n");
	}

}} // !etc::log
