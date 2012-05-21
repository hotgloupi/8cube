#ifndef  ETC_IO_LOGGER_HXX
# define ETC_IO_LOGGER_HXX

# include <etc/to_string.hpp>

namespace etc { namespace io {

    template<typename... T>
      void Logger::message(Level level, T const&... values)
      {
        if (level < this->_level)
          return;
        this->_message(level, etc::to_string(values...));
      }

    template<typename... T>
      void Logger::debug(T const&... values)
      {
        return this->message(Logger::Level::Debug, values...);
      }

    template<typename... T>
      void Logger::info(T const&... values)
      {
        return this->message(Logger::Level::Info, values...);
      }

    template<typename... T>
      void Logger::warn(T const&... values)
      {
        return this->message(Logger::Level::Warn, values...);
      }

    template<typename... T>
      void Logger::fatal(T const&... values)
      {
        return this->message(Logger::Level::Fatal, values...);
      }

}} // !etc::io

#endif
