#ifndef  ETC_LOG_HPP
# define ETC_LOG_HPP

# include <etc/log/Logger.hpp>

namespace etc { namespace log {

    etc::log::Logger& default_logger();

    template<typename... T> void debug(T const&... values)
    {
      default_logger().debug(values...);
    }

    template<typename... T> void info(T const&... values)
    {
      return default_logger().info(values...);
    }

    template<typename... T> void warn(T const&... values)
    {
      return default_logger().warn(values...);
    }

    template<typename... T> void error(T const&... values)
    {
      return default_logger().error(values...);
    }

    template<typename... T> void fatal(T const&... values)
    {
      return default_logger().fatal(values...);
    }

}}

#endif
