#include "Logger.hpp"
#include "detail/log.hpp"
#include "backend/Factory.hpp"
#include "Level.hpp"

#include <etc/meta/enum.hpp>
#include <etc/assert.hpp>
#include <etc/to_string.hpp>
#include <etc/memory.hpp>

#include <mutex>
#include <unordered_map>
#include <vector>

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
