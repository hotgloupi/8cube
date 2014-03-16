#ifndef  ETC_HTTP_RESPONSEIMPL_HPP
# define ETC_HTTP_RESPONSEIMPL_HPP

# include "Client.hpp"
# include "Request.hpp"
# include "Response.hpp"

# include <etc/exception.hpp>
# include <etc/scheduler/fwd.hpp>

# include <curl/curl.h>

namespace etc { namespace http {

	struct Response::Impl
	{
		Client& client;
		Request request;
		CURL* easy_handle;
		bool running;
		std::string in_data;
		scheduler::Context* context;

		Impl(Client& client, Request request);
		~Impl();

		static
		size_t _header_callback(void* data,
		                        size_t chunk_size,
		                        size_t count,
		                        void* userdata);
		static
		size_t _write_callback(char* data,
		                       size_t chunk_size,
		                       size_t count,
		                       void* userdata);
		static
		int _debug_callback(CURL* easy_handle,
		                    curl_infotype infotype,
		                    char* data, size_t size,
		                    void* userp);
		template<typename T>
		void _setopt(CURLoption opt, T&& value)
		{
			if (::curl_easy_setopt(this->easy_handle, opt, value) != CURLE_OK)
				throw exception::Exception{"Couldn't set cURL option"};
		}
	};

}}

#endif
