#ifndef  ETC_IO_LOGGER_HH
# define ETC_IO_LOGGER_HH

# include <boost/noncopyable.hpp>

namespace elle
{
  namespace utility
  {

    class Logger
      : private boost::noncopyable
    {
    public:
      enum class Level
      {
        Debug = 0,
        Info,
        Warn,
        Fatal,

        _MaxValue,
      };

    private:
      struct OutStream
      {
        std::ostream* out;
        bool          owned;
      };

    private:
      Level       _level;
      OutStream   _streams[static_cast<size_t>(Level::_MaxValue)];

    public:
      Logger(Level lvl);

    public:
      template<typename... T> void message(Level level, T const&...);
      template<typename... T> void debug(T const&...);
      template<typename... T> void info(T const&...);
      template<typename... T> void warning(T const&...);
      template<typename... T> void fatal(T const&...);

    protected:
      void _message(Level level, std::string const& message);
    };

  }
}

# include <elle/utility/Logger.hxx>

#endif /* ! LOGGER_HH */


