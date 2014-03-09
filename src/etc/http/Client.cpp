#include "ClientImpl.hpp"
#include "ResponseImpl.hpp"
#include "Request.hpp"

namespace etc { namespace http {

	ETC_LOG_COMPONENT("etc.http.Client");

	using exception::Exception;

	Client::Client(std::string server,
	               uint16_t port,
	               scheduler::Scheduler* sched)
		: _this{new Impl(std::move(server), port, sched)}
	{ ETC_TRACE_CTOR(); }

	Client::~Client()
	{ ETC_TRACE_DTOR(); }

	Response Client::fire(Request req)
	{
		return Response{
			etc::make_unique<Response::Impl>(*this, std::move(req))
		};
	}

	std::string const& Client::server() const ETC_NOEXCEPT
	{ return _this->server; }

	uint16_t Client::port() const ETC_NOEXCEPT
	{ return _this->port; }

}}
