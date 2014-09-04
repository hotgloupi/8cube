#include "component.hpp"

#include "detail/log.hpp"
#include "detail/Pattern.hpp"
#include "Logger.hpp"

#include <etc/sys/environ.hpp>

#include <mutex>
#include <unordered_map>
#include <vector>

#ifdef _WIN32
# include <wrappers/windows.hpp>
#else
# include <fnmatch.h>
#endif

namespace etc { namespace log {

	Component& component(std::string const& name)
	{
		static std::unordered_map<std::string, Component> components;
		static std::mutex mutex;
		std::lock_guard<std::mutex> guard{mutex};
		auto it = components.find(name);
		if (it != components.end())
			return it->second;
		static std::string envvar = etc::sys::environ::get("ETC_LOG", "*");
		static std::vector<detail::Pattern> patterns = detail::get_patterns(envvar);
		Component res = {name, default_level(), false, logger(name)};
		for (auto const& pattern: patterns)
		{
#ifdef _WIN32
			if (::PathMatchSpec(name.c_str(), pattern.str.c_str()) == TRUE)
#else
			if (::fnmatch(pattern.str.c_str(), name.c_str(), 0) == 0)
#endif
			{
				res.level = pattern.level;
				res.enabled = (pattern.op == detail::Pattern::add_match);
				break;
			}
		}
		detail::log(
			"Component", name,
			(res.enabled ? "enabled" : "disabled"),
			"for level", res.level
		);
		components.insert(std::make_pair(name, res));
		return components.at(name);
	}

}}
