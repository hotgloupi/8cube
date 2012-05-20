#ifndef  ETC_LOG_HPP
# define ETC_LOG_HPP

# include <etc/io/Logger.hpp>

namespace etc { namespace log {

    template<typename... T> void debug(T const&...);
    template<typename... T> void info(T const&...);
    template<typename... T> void warn(T const&...);
    template<typename... T> void fatal(T const&...);

    extern elle::utility::Logger default_logger;
}}

# include <etc/log.ipp>

#endif
