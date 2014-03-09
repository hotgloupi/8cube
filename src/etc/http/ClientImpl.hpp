#ifndef  ETC_HTTP_CLIENTIMPL_HPP
# define ETC_HTTP_CLIENTIMPL_HPP

# include "Client.hpp"

# include <etc/log.hpp>
# include <etc/exception.hpp>

# include <curl/curl.h>

namespace etc { namespace http {

	struct Client::Impl
	{
		ETC_LOG_COMPONENT("etc.http.ClientImpl");

		std::string const server;
		uint16_t const port;
		scheduler::Scheduler* sched;
		CURLM* multi_handle;

		Impl(std::string server,
		     uint16_t port,
		     scheduler::Scheduler* sched)
			: server{std::move(server)}
			, port{port}
			, sched{sched}
			, multi_handle{::curl_multi_init()}
		{
			ETC_TRACE_CTOR();
			if (this->sched == nullptr)
				throw exception::Exception{"No scheduler available"};
			if (this->multi_handle == nullptr)
				throw exception::Exception{"Couldn't create a multi_handle"};
		}

		~Impl()
		{
			ETC_TRACE_DTOR();
			::curl_multi_cleanup(this->multi_handle);
		}
	};

}}

#endif
