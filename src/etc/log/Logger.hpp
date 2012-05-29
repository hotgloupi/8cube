#ifndef  ETC_IO_LOGGER_HH
# define ETC_IO_LOGGER_HH

# include <iosfwd>

# include <boost/noncopyable.hpp>

# include <etc/to_string.hpp>

namespace etc { namespace log {

    class Logger
      : private boost::noncopyable
    {
    public:
      enum class Level
      {
        debug = 0,
        info,
        warn,
        error,
        fatal,

        _maxvalue,
      };

    private:
      struct OutStream
      {
        std::ostream* out;
        bool          owned;
      };

    private:
      Level       _level;
      OutStream   _streams[static_cast<size_t>(Level::_maxvalue)];

    public:
      Logger(Level lvl);

    public:
      template<typename... T> void message(Level level, T const&... values)
      {
        if (level < this->_level)
          return;
        this->_message(level, etc::to_string(values...));
      }

      template<typename... T> void debug(T const&... values)
          { return this->message(Logger::Level::debug, values...); }

      template<typename... T> void info(T const&... values)
          { return this->message(Logger::Level::info, values...); }

      template<typename... T> void warn(T const&... values)
          { return this->message(Logger::Level::warn, values...); }

      template<typename... T> void error(T const&... values)
        { return this->message(Logger::Level::error, values...);  }

      template<typename... T> void fatal(T const&... values)
        { return this->message(Logger::Level::fatal, values...);  }

    protected:
      void _message(Level level, std::string const& message);
    };

  }
}

#endif


