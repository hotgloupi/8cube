#include "ResponseImpl.hpp"
#include "ClientImpl.hpp"

#include <etc/log.hpp>
#include <etc/scheduler/Context.hpp>
#include <etc/scheduler/SchedulerImpl.hpp>
#include <etc/scope_exit.hpp>

namespace etc { namespace http {

	ETC_LOG_COMPONENT("etc.http.Response");

	Response::Impl::Impl(Client& client, Request request)
		: client(client)
		, request(std::move(request))
		, easy_handle{::curl_easy_init()}
		, running{true}
		, context{nullptr}
	{
		ETC_TRACE_CTOR();
		if (this->easy_handle == nullptr)
			throw exception::Exception{"Couldn't create an easy_handle"};

		_setopt(CURLOPT_VERBOSE, 1);
		_setopt(CURLOPT_DEBUGFUNCTION, &_debug_callback);
		_setopt(CURLOPT_DEBUGDATA, this);
		_setopt(CURLOPT_NOSIGNAL, 1);
		_setopt(CURLOPT_TIMEOUT, 10);

		// Prevent a throwing constructor to let the easy handle alive.
		auto handle_guard = etc::scope_exit(
			[this] { ::curl_easy_cleanup(this->easy_handle); }
		);

		ETC_LOG.debug(*this, "Set HTTP method to", this->request.method());
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
			throw exception::Exception{
				"Unsupported HTTP method " + etc::to_string(this->request.method())
			};
		}

		// url

		// url parameters
		{
			std::string uri;
			{
				std::string srv = "LOL";//this->client.server();
				std::string url = this->request.url();
				if (url == "/") // Ignore empty urls
					uri = srv;
				else if (!srv.empty() && srv.back() == '/') // no double /
					uri = srv + url.substr(1, std::string::npos);
				else
					uri = srv + url;
			}

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
			ETC_LOG.debug(*this, "Set URI to", uri);
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
				ETC_LOG.debug(*this, "Add header", line);
				headers = ::curl_slist_append(headers, line.c_str());
			}
			_setopt(CURLOPT_HTTPHEADER, headers);
		}

		// Callbacks
		_setopt(CURLOPT_WRITEFUNCTION, &_write_callback);
		_setopt(CURLOPT_WRITEDATA, this);
		_setopt(CURLOPT_HEADERFUNCTION, &_header_callback);
		_setopt(CURLOPT_HEADERDATA, this);
		if (::curl_multi_add_handle(this->client.impl().multi_handle,
		                            this->easy_handle) != CURLM_OK)
			throw exception::Exception{"Couldn't register the easy handle"};
		handle_guard.dismiss();
	}

	Response::Impl::~Impl()
	{
		ETC_TRACE_DTOR();
		ETC_TRACE.debug(*this, "Removing easy handle", easy_handle, "from cURL");
		::curl_multi_remove_handle(this->client.impl().multi_handle, easy_handle);
		::curl_easy_cleanup(this->easy_handle);
	}

	size_t Response::Impl::_header_callback(void* data,
	                                        size_t chunk_size,
	                                        size_t count,
	                                        void* userdata)
	{
		Impl& self = *(Impl*) userdata;
		ETC_LOG.debug(self, "Received", chunk_size * count, "bytes of headers");
		// XXX Parse response code here
		return chunk_size * count;
	}

	size_t Response::Impl::_write_callback(char* data,
	                                       size_t chunk_size,
	                                       size_t count,
	                                       void* userdata)
	{
		Impl& self = *(Impl*) userdata;
		ETC_TRACE.debug(self, "Got", count * chunk_size, "bytes");
		if (self.context != nullptr)
		{
			self.in_data.append(data, chunk_size * count);
			self.client.impl().sched.impl().wakeup(*self.context);
		}
		else
		{
			ETC_LOG.warn(self, "Nobody is awaiting for data, dropping it");
		}
		return chunk_size * count; // bytes actually taken care of.
	}

	int Response::Impl::_debug_callback(CURL* easy_handle,
	                                    curl_infotype infotype,
	                                    char* data, size_t size,
	                                    void* userp)
	{
		auto& self = *(Impl*) userp;
		switch (infotype)
		{
		case CURLINFO_TEXT:
			ETC_LOG.debug("CURL:", self, "on", easy_handle, std::string{data, size});
			break;
		case CURLINFO_HEADER_IN:
			ETC_LOG.debug("CURL:", self, "on", easy_handle, "got headers:\n" +
			              std::string{data, size});
			break;
		case CURLINFO_HEADER_OUT:
			ETC_LOG.debug("CURL:", self, "on", easy_handle, "sent headers:\n" +
			              std::string{data, size});
			break;
		case CURLINFO_DATA_IN:
			ETC_LOG.debug("CURL:", self, "on", easy_handle, "got", size, "bytes");
			//ETC_LOG.debug(self, "on", easy_handle, "got data:\n" +
			//              std::string{data, size});
			break;
		case CURLINFO_DATA_OUT:
			ETC_LOG.debug("CURL:", self, "on", easy_handle, "sent", size, "bytes");
			//ETC_LOG.debug(self, "on", easy_handle, "sent data:\n" +
			//              std::string{data, size});
			break;
		case CURLINFO_SSL_DATA_OUT:
			ETC_LOG.debug("CURL:", self, "on", easy_handle, "sent ssl", size, "bytes");
			//ETC_LOG.debug(self, "on", easy_handle, "sent ssl data:\n" +
			//              std::string{data, size});
			break;
		case CURLINFO_SSL_DATA_IN:
			ETC_LOG.debug("CURL:", self, "on", easy_handle, "got ssl", size, "bytes");
			//ETC_LOG.debug(self, "on", easy_handle, "got ssl data:\n" +
			//              std::string{data, size});
			break;
		default:
			ETC_LOG.debug("CURL:", self, "on", easy_handle,
			              "has unknown debug info type:\n" +
			              std::string{data, size});
			break;
		}
		return 0;
	}


}}
