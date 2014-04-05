#ifndef  ETC_LOG_INIT_HPP
# define ETC_LOG_INIT_HPP

# include <etc/api.hpp>

# include <memory>

namespace etc { namespace log {

	/// Initialize and shutdown etc::log.
	struct ETC_API Init
	{
		Init();
		~Init();
	};

}}

#endif

