#include "ResponseImpl.hpp"

#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>

namespace etc { namespace http {

	Response::Response(std::unique_ptr<Impl> impl)
		: _this{std::move(impl)}
	{}

	Response::Response(Response&& other)
		: _this{std::move(other._this)}
	{}

	Response::~Response()
	{}

	namespace {

		struct ResponseBody
			: public boost::iostreams::source
		{
			Response& response;
			Client& client;

			ResponseBody(Response& response)
				: response(response)
				, client(response.impl().client)
			{
			}

			std::streamsize read(char* s, std::streamsize n)
			{
				return 0;
			}
		};

		typedef boost::iostreams::stream<ResponseBody> ResponseBodyStream;
	}

	std::istream& Response::body()
	{
		if (_this->body == nullptr)
			_this->body.reset(new ResponseBodyStream{*this});
		return *_this->body;
	}

}}
