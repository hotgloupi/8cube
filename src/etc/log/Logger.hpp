#ifndef  ETC_IO_LOGGER_HH
# define ETC_IO_LOGGER_HH

# include "Line.hpp"

# include "../api.hpp"
# include "../to_string.hpp"

# include <boost/noncopyable.hpp>

# include <iosfwd>

namespace etc { namespace log {

	enum class Flag : int
	{
		none        = 0x00,
		level       = 0x01,
		component   = 0x02,
		location    = 0x04,
		function    = 0x08,
	};

	inline
	Flag operator |(Flag const lhs, Flag const rhs)
	{
		return static_cast<Flag>(
			static_cast<int>(lhs) | static_cast<int>(rhs)
		);
	}

	inline
	bool operator &(Flag const lhs, Flag const rhs)
	{
		// C4800 warning when casting to bool
		if (static_cast<int>(lhs) & static_cast<int>(rhs))
			return true;
		return false;
	}

	class Logger;
	struct Line;

	/// Factory for loggers
	ETC_API Logger& logger(std::string const& name = "");

	ETC_API void shutdown();

	class ETC_API Logger
		: private boost::noncopyable
	{
	public:

	private:
		struct OutStream
		{
			std::ostream* out;
			bool          owned;

			OutStream()
				: out{nullptr}
				, owned{false}
			{}

			OutStream(std::ostream* out, bool owned)
				: out{out}
				, owned{owned}
			{}
		};

		struct ComponentConfig
		{
			Level level;
			bool enabled;
		};

		static
		ComponentConfig const& _component_config(std::string const& name);

		inline
		bool _should_log(Line const& line)
		{
			if (line.level < _level) // lesser than global log level
			{
				auto const& config = _component_config(line.component);
				if (line.level < config.level || !config.enabled)
					return false;
			}
			return true;
		}
	private:
		std::string     _name;
		Level           _level;
		OutStream       _streams[static_cast<size_t>(Level::_maxvalue)];
		Flag            _flags;

	private:
		Logger(std::string const& name,
		       Level lvl = Level::debug,
		       Flag const flags = Flag::level | Flag::component);
		friend ETC_API Logger& logger(std::string const&);

	public:
		inline
		void message(Line const& line, std::string msg) noexcept
		{ if (_should_log(line)) this->_message(line, std::move(msg)); }

		template<typename... T>
		inline
		void message(Line const& line, T const&... values) noexcept
		{
			if (_should_log(line))
				this->_message(line, etc::to_string(values...));
		}

	protected:
		void _message(Line const& line, std::string&& message) noexcept;
	};

}} // !etc::log

# include <boost/config.hpp>

# ifdef BOOST_MSVC
#  define _ETC_LOG_FUNCTION __FUNCTION__
# else
#  define _ETC_LOG_FUNCTION __PRETTY_FUNCTION__
# endif

#endif
