#ifndef  ETC_HTTP_INIT_HPP
# define ETC_HTTP_INIT_HPP

# include <memory>

namespace etc { namespace http {

	/// Initialize and shutdown etc::http.
	struct Init
	{
		Init();
		~Init();
	};

}}

#endif


