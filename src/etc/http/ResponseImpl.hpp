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
		std::unique_ptr<std::istream> body;

		Impl(Client& client, Request request)
			: client(client)
			, request(std::move(request))
			, easy_handle{::curl_easy_init()}
		{
			if (this->easy_handle == nullptr)
				throw exception::Exception{"Couldn't create an easy_handle"};

			// Prevent a throwing constructor to let the easy handle alive.
			auto handle_guard = etc::scope_exit(
				[this] { ::curl_easy_cleanup(this->easy_handle); }
			);

			// Set HTTP method
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

			// url parameters
			{
				std::string uri = this->client.server() + this->request.url();

				bool first = true;
				for (auto parameter: this->request.parameters())
				{
					if (parameter.first.empty())
						continue;
					if (first)
					{
						first = false;
						uri.push_back('?');
					}
					else
						uri.push_back('&');
					char* key = ::curl_easy_escape(this->easy_handle,
					                               parameter.first.c_str(),
					                               parameter.first.size());
					ETC_SCOPE_EXIT{ ::curl_free(key); };
					uri.append(key);

					if (parameter.second.empty())
						continue;
					uri.push_back('=');
					char* value = ::curl_easy_escape(this->easy_handle,
					                                 parameter.second.c_str(),
					                                 parameter.second.size());
					ETC_SCOPE_EXIT{ ::curl_free(value); };
					uri.append(value);
				}
				_setopt(CURLOPT_URL, uri.c_str());
			}

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

			// Callbacks
			_setopt(CURLOPT_WRITEFUNCTION, &_write_function);
			_setopt(CURLOPT_WRITEDATA, this);

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
			::curl_easy_cleanup(this->easy_handle);
		}

	};

}}

#endif
