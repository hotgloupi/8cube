#include "Pattern.hpp"
#include "log.hpp"

#include <etc/log/Level.hpp>

#include <etc/sys/environ.hpp>

#include <boost/algorithm/string.hpp>

namespace etc { namespace log { namespace detail {

	std::vector<Pattern> get_patterns(std::string const& envvar)
	{
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
			std::vector<std::string> parts;
			boost::algorithm::split(
				parts,
				pattern,
				boost::algorithm::is_any_of(":"),
				boost::algorithm::token_compress_on
			);
			if (parts.size() == 0)
			{
				detail::log("Ignoring empty log pattern");
				continue;
			}
			else if (parts.size() > 2)
			{
				detail::log("Ignoring invalid log pattern:", "'" + pattern + "'");
				continue;
			}
			std::string match = parts[0];
			Level level = default_level();
			if (parts.size() == 2)
			{
				try { level = level_from_string(parts[1]); }
				catch (std::out_of_range const&)
				{
					log("Unknown level string", parts[1]);
					level = default_level();
				}
			}
			detail::log("match:", match, "level:", level);

			if (match.size() == 0 ||
					(match.size() == 1 && (
						match[0] == '+' ||
						match[0] == '-')))
			{
				detail::log("Invalid match expression:", "'" + match + "'");
				continue;
			}
			if (match[0] == '+')
				res.emplace_back(
					Pattern::add_match,
					"*" + match.substr(1) + "*",
					level
				);
			else if (match[0] == '-')
				res.emplace_back(
					Pattern::remove_match,
					"*" + match.substr(1) + "*",
					level
				);
			else
				res.emplace_back(
					Pattern::add_match,
					"*" + match + "*",
					level
				);
		}
		for (auto const& p: res)
			detail::log("Pattern:", p.level, p.str, p.op);
		return res;
	}

}}}
