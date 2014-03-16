#ifndef  ETC_HTTP_CLIENTIMPL_HPP
# define ETC_HTTP_CLIENTIMPL_HPP

# include "Client.hpp"
# include "ResponseImpl.hpp"

# include <etc/log.hpp>
# include <etc/exception.hpp>

# include <curl/curl.h>

# include <boost/asio/ip/tcp.hpp>

# include <map>
# include <set>

namespace etc { namespace http {

	struct Client::Impl
	{
		ETC_LOG_COMPONENT("etc.http.ClientImpl");

		std::string const server;
		uint16_t const port;
		scheduler::Scheduler& sched;
		CURLM* multi_handle;
		std::map<CURL*, boost::asio::ip::tcp::socket> sockets;
		std::map<CURL*, std::shared_ptr<Response::Impl>> responses;
		std::set<boost::asio::deadline_timer*> timers;
		int running_handles;

		Impl(std::string server,
		     uint16_t port,
		     scheduler::Scheduler& sched);

		void add_handle(std::shared_ptr<Response::Impl>& response);

		void setup_handle(CURL* easy_handle, curl_socket_t fd);
		void remove_handle(CURL* easy_handle);

		void setup_read_check(CURL* easy_handle);
		void setup_write_check(CURL* easy_handle);

		void _check_multi_info();

		static
		int _socket_callback(CURL *easy,      /* easy handle */
		                     curl_socket_t s, /* socket */
		                     int action,      /* see values below */
		                     void *userp,    /* private callback pointer */
		                     void *socketp);

		static
		int _timer_callback(CURLM* multi /* multi handle */ ,
		                    long timeout_ms /* timeout in milliseconds */ ,
		                    void* userp /* TIMERDATA */ );
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
