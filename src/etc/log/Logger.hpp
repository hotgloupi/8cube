#ifndef  ETC_IO_LOGGER_HH
# define ETC_IO_LOGGER_HH

# include "Line.hpp"

# include <etc/to_string.hpp>

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
	Logger& logger(std::string const& name = "");

	class Logger
		: private boost::noncopyable
	{
	public:

	private:
		struct OutStream
		{
			std::ostream* out;
			bool          owned;
		};

	private:
		std::string     _name;
		Level           _level;
		OutStream       _streams[static_cast<size_t>(Level::_maxvalue)];
		Flag            _flags;

	private:
		Logger(std::string const& name,
		       Level lvl = Level::debug,
		       Flag const flags = Flag::level | Flag::component);
		friend Logger& logger(std::string const&);

	public:
		template<typename... T>
		void message(Line const& line, T const&... values)
		{
			if (line.level < _level)
				return;
			this->_message(line, etc::to_string(values...));
		}

	protected:
		void _message(Line const& line, std::string const& message);
	};

}} // !etc::log

# include <boost/config.hpp>

# ifdef BOOST_MSVC
#  define _ETC_LOG_FUNCTION __FUNCTION__
# else
#  define _ETC_LOG_FUNCTION __PRETTY_FUNCTION__
# endif

#endif
