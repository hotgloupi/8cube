#ifndef  ETC_HTTP_CLIENTIMPL_HPP
# define ETC_HTTP_CLIENTIMPL_HPP

# include "Client.hpp"

# include <etc/log.hpp>
# include <etc/exception.hpp>

# include <curl/curl.h>

# include <boost/asio/ip/tcp.hpp>

# include <map>

namespace etc { namespace http {

	struct Client::Impl
	{
		ETC_LOG_COMPONENT("etc.http.ClientImpl");

		std::string const server;
		uint16_t const port;
		scheduler::Scheduler& sched;
		CURLM* multi_handle;
		std::map<CURL*, boost::asio::ip::tcp::socket> easy_handles;

		Impl(std::string server,
		     uint16_t port,
		     scheduler::Scheduler& sched);

		void add_handle(CURL* easy_handle);

		void setup_handle(CURL* easy_handle, curl_socket_t fd);
		void remove_handle(CURL* easy_handle);

		void setup_read_check(CURL* easy_handle);

		static
		int _socket_function(CURL *easy,      /* easy handle */
		                     curl_socket_t s, /* socket */
		                     int action,      /* see values below */
		                     void *userp,    /* private callback pointer */
		                     void *socketp);

		~Impl();

		template<typename T>
		void _setopt(CURLMoption opt, T&& value)
		{
			if (::curl_multi_setopt(this->multi_handle,
			                        opt,
			                        value) != CURLM_OK)
				throw exception::Exception{"Couldn't set cURL option"};
		}
	};

}}

#endif
