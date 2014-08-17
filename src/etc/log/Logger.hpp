#ifndef  ETC_IO_LOGGER_HH
# define ETC_IO_LOGGER_HH

# include "fwd.hpp"

# include "Line.hpp"
# include "backend/Interface.hpp"
# include "detail/Runner.hpp"

# include <etc/api.hpp>

# include <boost/noncopyable.hpp>

# include <iosfwd>
# include <vector>

namespace etc { namespace log {

	class Logger;
	struct Line;

	/// Factory for loggers
	ETC_API Logger& logger(std::string const& name = "");

	ETC_API void shutdown();

	ETC_API void set_mode(Mode mode);

	class ETC_API Logger
		: private boost::noncopyable
	{
	private:
		struct ComponentConfig
		{
			Level level;
			bool enabled;
		};

		static
		ComponentConfig& _component_config(std::string const& name);

	public:
		inline
		bool should_log(Line const& line)
		{
			if (line.level < _level) // lesser than global log level
			{
				auto const& config = _component_config(line.component);
				if (line.level < config.level || !config.enabled)
					return false;
			}
			return true;
		}

		static
		void enable_component(std::string const& name,
		                      Level lvl = Level::_maxvalue)
		{
			if (lvl == Level::_maxvalue)
				lvl = logger(name)._level;
			_component_config(name).level = lvl;
			_component_config(name).enabled = true;
		}

		static
		void disable_component(std::string const& name)
		{ _component_config(name).enabled = false; }

	private:
		std::string                                      _name;
		Level                                            _level;
		std::vector<std::shared_ptr<backend::Interface>> _backends;

	private:
		explicit
		Logger(std::string const& name,
		       Level lvl = Level::debug);
		friend ETC_API Logger& logger(std::string const&);

	public:
		Level level() const ETC_NOEXCEPT { return _level; }
		Logger& level(Level const level) { _level = level; return *this; }

	public:
		void add_backend(std::shared_ptr<backend::Interface> b)
		{ _backends.emplace_back(std::move(b)); }

	public:
		void message(Line line, std::string message) ETC_NOEXCEPT;
	};

}} // !etc::log

#endif
