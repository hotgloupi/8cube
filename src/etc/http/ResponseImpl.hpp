#ifndef  ETC_HTTP_RESPONSEIMPL_HPP
# define ETC_HTTP_RESPONSEIMPL_HPP

# include "Response.hpp"
# include "Request.hpp"
# include "ClientImpl.hpp"

# include <etc/exception.hpp>
# include <etc/scope_exit.hpp>

# include <curl/curl.h>

namespace etc { namespace http {

	struct Response::Impl
	{
		Client& client;
		Request request;
		CURL* easy_handle;

		Impl(Client& client, Request request)
			: client(client)
			, request(std::move(request))
			, easy_handle{::curl_easy_init()}
		{
			if (this->easy_handle == nullptr)
				throw exception::Exception{"Couldn't create an easy_handle"};

			auto handle_guard = etc::scope_exit(
				[this] { ::curl_easy_cleanup(this->easy_handle); }
			);

			// method
			switch (this->request.method())
			{
			case Method::head:
				_setopt(CURLOPT_NOBODY, 1); break;
			case Method::get:
				_setopt(CURLOPT_HTTPGET, 1); break;
			case Method::post:
				_setopt(CURLOPT_HTTPPOST, 1); break;
			default:
				throw exception::Exception{"Unsupported HTTP method"};
			}

			// url
			std::string uri = this->client.server() + this->request.url();
			_setopt(CURLOPT_URL, uri.c_str());

			// headers
			{
				struct curl_slist* headers = nullptr;
				// XXX Should we keep the list alive ?
				ETC_SCOPE_EXIT{ ::curl_slist_free_all(headers); };
				for (auto header: this->request.headers())
				{
					auto line = header.first + ": " + header.second;
					headers = ::curl_slist_append(headers, line.c_str());
				}
				_setopt(CURLOPT_HTTPHEADER, headers);
			}

			// Callback
			_setopt(CURLOPT_WRITEFUNCTION, &_write_function);
			_setopt(CURLOPT_WRITEDATA, this);

			// This must be the last thing we do (easy handle is set up).
			if (::curl_multi_add_handle(client.impl().multi_handle,
			                            this->easy_handle) != CURLM_OK)
				throw exception::Exception{"Couldn't register the easy handle"};
			handle_guard.dismiss();
		}

		static
		size_t _write_function(char* data,
		                       size_t chunk_size,
		                       size_t count,
		                       void* userdata)
		{
			Impl *const self = (Impl*) userdata;

			return 0; // bytes actually taken care of.
		}

		template<typename T>
		void _setopt(CURLoption opt, T&& value)
		{
			if (::curl_easy_setopt(this->easy_handle, opt, value) != CURLE_OK)
				throw exception::Exception{"Couldn't set cURL option"};
		}

		~Impl()
		{
			::curl_multi_remove_handle(this->client.impl().multi_handle,
			                           this->easy_handle);
			::curl_easy_cleanup(this->easy_handle);
		}

	};

}}

#endif
