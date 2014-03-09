#ifndef  ETC_LOG_INIT_HPP
# define ETC_LOG_INIT_HPP

# include <memory>

namespace etc { namespace log {

	/// Initialize and shutdown etc::log.
	struct Init
	{
		Init();
		~Init();
	};

}}

#endif

