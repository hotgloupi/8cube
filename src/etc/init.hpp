#ifndef  ETC_INIT_HPP
# define ETC_INIT_HPP

# include "api.hpp"
# include "log/init.hpp"
# include "http/init.hpp"

namespace etc {

	// Initialize and shutdown etc library.
	struct ETC_API Init
	{
		Init();
		~Init();
		log::Init  init_guard_log;
		http::Init init_guard_http;
	};

}

#endif
