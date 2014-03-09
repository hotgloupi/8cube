#include "ResponseImpl.hpp"

namespace etc { namespace http {

	Response::Response(std::unique_ptr<Impl> impl)
		: _this{std::move(impl)}
	{}

	Response::Response(Response&& other)
		: _this{std::move(other._this)}
	{}

	Response::~Response()
	{}

}}
