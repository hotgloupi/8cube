#ifndef  ETC_LOG_HXX
# define ETC_LOG_HXX

namespace etc { namespace log {

    template<typename... T> void debug(T const&... values)
    {
      return default_logger.debug(values...);
    }

    template<typename... T> void info(T const&... values)
    {
      return default_logger.info(values...);
    }

    template<typename... T> void warn(T const&... values)
    {
      return default_logger.warn(values...);
    }

    template<typename... T> void fatal(T const&... values)
    {
      return default_logger.fatal(values...);
    }

}} // !etc::log

#endif
