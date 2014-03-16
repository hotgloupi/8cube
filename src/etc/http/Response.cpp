#include "ResponseImpl.hpp"
#include "ClientImpl.hpp"

#include <etc/log.hpp>
#include <etc/scheduler.hpp>
#include <etc/scheduler/SchedulerImpl.hpp>
#include <etc/scheduler/Context.hpp>

#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>

namespace etc { namespace http {

	ETC_LOG_COMPONENT("etc.http.Response");

	Response::Response(std::shared_ptr<Impl> impl)
		: _this{std::move(impl)}
	{ ETC_TRACE_CTOR(); }

	Response::Response(Response&& other)
		: _this{std::move(other._this)}
	{ ETC_TRACE_CTOR("by stealing", other); }

	Response::~Response()
	{ ETC_TRACE_DTOR(); }

	ResponseCode Response::code()
	{
		ETC_TRACE.debug(*this, "Fetching response code");

		// XXX curl_easy_getinfo does not work for transfer in-progress ...
		while (true)
		{
			long http_code;
			CURLcode ret = curl_easy_getinfo(_this->easy_handle,
			                                  CURLINFO_RESPONSE_CODE,
			                                  &http_code);
			if (ret == CURLE_OK && http_code != 0)
			{
				ETC_LOG.debug("Got response code", http_code);
				return (ResponseCode) http_code;
			}
			if (!_this->running)
				throw exception::Exception{"Couldn't retreive HTTP code"};

			ETC_LOG.debug(*this, "Couldn't get the response code, yield");
			// Try again while we're running, let curl do its job.
			_this->client.impl().sched.context().yield();
		}
	}

	std::string Response::read(size_t size)
	{
		ETC_TRACE.debug(
			*this, "reading",
			size == 0 ? "until the end": "up to " + std::to_string(size) + "bytes"
		);
		std::string res;
		auto& ctx = scheduler::current().context();
		if (_this->context != nullptr)
			throw exception::Exception{"Somedy already reading this response"};
		_this->context = &ctx;
		ETC_SCOPE_EXIT{ _this->context = nullptr; };
		while (true)
		{
			if (_this->in_data.empty())
			{
				ETC_TRACE.debug("Nothing to read, going to sleep");
				scheduler::current().impl().freeze(ctx);
				ctx.yield();
			}
			if (size != 0)
			{
				size_t to_copy = std::min(_this->in_data.size(), size - res.size());
				ETC_LOG.debug(*this, "Copying", to_copy, "bytes");
				res.append(_this->in_data.substr(0, to_copy));
				_this->in_data = _this->in_data.substr(to_copy, std::string::npos);
			}
			else
			{
				ETC_LOG.debug(*this, "Copying", _this->in_data.size(), "bytes");
				res.append(_this->in_data);
				_this->in_data.clear();
			}
			if (_this->running == false || (size != 0 && res.size() == size))
			{
				ETC_LOG.debug("Read terminated with", res.size(), "bytes");
				break;
			}
		}
		return res;
	}

	//namespace {

	//	struct ResponseBody
	//		: public boost::iostreams::source
	//	{
	//		Response& response;
	//		Client& client;

	//		ResponseBody(Response& response)
	//			: response(response)
	//			, client(response.impl().client)
	//		{
	//		}

	//		std::streamsize read(char* s, std::streamsize n)
	//		{
	//			return 0;
	//		}
	//	};

	//	typedef boost::iostreams::stream<ResponseBody> ResponseBodyStream;
	//}

	//std::istream& Response::body()
	//{
	//	if (_this->body == nullptr)
	//		_this->body.reset(new ResponseBodyStream{*this});
	//	return *_this->body;
	//}

}}
