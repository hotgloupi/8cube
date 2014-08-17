#include "Level.hpp"
#include "detail/log.hpp"

#include <boost/algorithm/string.hpp>

#include <etc/sys/environ.hpp>

#include <ostream>
#include <unordered_map>

namespace etc { namespace log {

	std::ostream& operator <<(std::ostream& out, Level level)
	{
		static char const* strs[] = {
			"DEBUG",
			"INFO",
			"WARNING",
			"ERROR",
			"FATAL",
		};
		return (out << strs[(unsigned int) level]);
	}

	Level level_from_string(std::string const& str,
	                        bool ignore_case)
	{
		static std::unordered_map<std::string, Level> map{
			{"DEBUG", Level::debug},
			{"INFO", Level::info},
			{"WARN", Level::warn},
			{"WARNING", Level::warn},
			{"ERROR", Level::error},
			{"FATAL", Level::fatal},
		};
		if (ignore_case)
			return map.at(boost::algorithm::to_upper_copy(str));
		return map.at(str);
	}


	Level default_level()
	{
		static std::string level_string =
			etc::sys::environ::get("ETC_LOG_LEVEL", "INFO");
		static Level value = Level::_maxvalue;
		if (value == Level::_maxvalue)
			try { value = level_from_string(level_string); }
			catch (std::out_of_range const&)
			{
				detail::log(
					"Unkwnown log level '" + level_string + "':",
					"defaulting to", Level::info
				);
				value = Level::info;
			}
		return value;
	}

}}
