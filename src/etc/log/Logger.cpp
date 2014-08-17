#include "Logger.hpp"
#include "detail/log.hpp"
#include "detail/Pattern.hpp"
#include "backend/Factory.hpp"
#include "Level.hpp"

#include <etc/meta/enum.hpp>
#include <etc/assert.hpp>
#include <etc/sys/environ.hpp>
#include <etc/to_string.hpp>
#include <etc/memory.hpp>

#include <mutex>
#include <unordered_map>
#include <vector>

#ifdef _WIN32
# include <wrappers/windows.hpp>
#else
# include <fnmatch.h>
#endif
#define ETC_DEBUG

namespace etc { namespace log {

	Logger& logger(std::string const& name)
	{
		static std::unordered_map<std::string, std::unique_ptr<Logger>> loggers;
		static Level level = default_level();
		static std::mutex mutex;
		std::lock_guard<std::mutex> guard{mutex};
		auto it = loggers.find(name);
		if (it != loggers.end())
			return *it->second;
		std::unique_ptr<Logger> ptr{new Logger(name, level)};
		ptr->add_backend(backend::factory().default_backend());
		return *(loggers[name] = std::move(ptr));
	}

	void shutdown()
	{
		// XXX shutdown all backends.
		backend::factory().default_backend()->shutdown();
	}

	void set_mode(Mode mode)
	{
		detail::log("Set logger mode to", mode);
		// XXX do it for all backends.
		backend::factory().default_backend()->set_mode(mode);
	}

	Logger::Logger(std::string const& name,
	               Level level)
		: _name(name)
		, _level(level)
	{
		detail::log("Creating logger for", name, "with level", level);
	}

	template<typename Config>
	struct ComponentConfigMap
	{
		std::unordered_map<std::string, Config> _components;

		ComponentConfigMap() : _components{}
		{ detail::log("Creating ComponentConfigMap"); }
		~ComponentConfigMap()
		{ detail::log("Destroying ComponentConfigMap"); }

#define FORWARD_METHOD(__method) \
		template<typename... Args> \
		auto __method(Args&&... args) const \
			-> decltype(_components.__method(std::forward<Args>(args)...)) \
		{ return _components.__method(std::forward<Args>(args)...); } \
		template<typename... Args> \
		auto __method(Args&&... args) \
			-> decltype(_components.__method(std::forward<Args>(args)...)) \
		{ return _components.__method(std::forward<Args>(args)...); } \
/**/

		FORWARD_METHOD(find);
		FORWARD_METHOD(begin);
		FORWARD_METHOD(end);
		FORWARD_METHOD(operator []);
	};

	Logger::ComponentConfig&
	Logger::_component_config(std::string const& name)
	{
		static ComponentConfigMap<ComponentConfig> components;
		auto it = components.find(name);
		if (it != components.end())
			return it->second;
		static std::string envvar = etc::sys::environ::get("ETC_LOG", "*");
		static std::vector<detail::Pattern> patterns = detail::get_patterns(envvar);
		ComponentConfig res = {default_level(), false};
		for (auto const& pattern: patterns)
		{
#ifdef _WIN32
			if (::PathMatchSpec(name.c_str(), pattern.str.c_str()) == TRUE)
#else
			if (::fnmatch(pattern.str.c_str(), name.c_str(), 0) == 0)
#endif
			{
				res = {pattern.level, (pattern.op == detail::Pattern::add_match)};
				break;
			}
		}
		detail::log(
			"Component", name,
			(res.enabled ? "enabled" : "disabled"),
			"for level", res.level
		);
		return components[name] = res;
	}

	void Logger::message(Line line,
	                     std::string message) ETC_NOEXCEPT
	{
		ETC_ASSERT(!_backends.empty());
		try
		{
			for (etc::size_type i = 1, len = _backends.size(); i < len; ++i)
			{
				auto& b = _backends[i];
				b->send_line(line, message);
			}
			auto& b = _backends[0];
			b->send_line(std::move(line), std::move(message));
		}
		catch (...)
		{
			detail::log("Couldn't log a message:", etc::exception::string());
		}
	}

}} // !etc::log
