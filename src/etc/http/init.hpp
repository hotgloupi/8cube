#ifndef  ETC_HTTP_INIT_HPP
# define ETC_HTTP_INIT_HPP

# include <etc/api.hpp>

# include <memory>

namespace etc { namespace http {

	/// Initialize and shutdown etc::http.
	struct ETC_API Init
	{
		Init();
		~Init();
	};

}}

#endif


