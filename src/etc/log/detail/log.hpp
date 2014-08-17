#ifndef  ETC_LOG_DETAIL_LOG_HPP
# define ETC_LOG_DETAIL_LOG_HPP

# include <etc/print.hpp>
# include <etc/sys/environ.hpp>

namespace etc { namespace log { namespace detail {

	template<typename... Args>
	void log(Args&&... args)
	{
		static bool debug_logger = sys::environ::as<bool>("ETC_LOG_DEBUG");
		if (debug_logger)
			etc::sprint(std::cerr, "[LOGGER]", std::forward<Args>(args)...);
	}

}}}

#endif
