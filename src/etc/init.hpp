#ifndef  ETC_INIT_HPP
# define ETC_INIT_HPP

# include "log/init.hpp"
# include "http/init.hpp"

namespace etc {

	// Initialize and shutdown etc library.
	struct Init
	{
		log::Init  init_guard_log;
		http::Init init_guard_http;
	};

}

#endif
