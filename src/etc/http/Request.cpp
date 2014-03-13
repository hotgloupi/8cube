#include "Request.hpp"

#include <etc/log.hpp>
#include <etc/test.hpp>

namespace etc { namespace http {

	ETC_LOG_COMPONENT("etc.http.Request");

	Request::Request(Method method)
		: _url{"/"}
		, _parameters{}
		, _method{method}
		, _headers{}
		, _body{}
	{ ETC_TRACE_CTOR(); }

	Request::Request(Request const& other)
		: _url{other._url}
		, _parameters{other._parameters}
		, _headers{other._headers}
		, _body{other._body}
	{ ETC_TRACE_CTOR("by copying", other); }

	Request::Request(Request&& other)
		: _url{std::move(other._url)}
		, _parameters{std::move(other._parameters)}
		, _headers{std::move(other._headers)}
		, _body{std::move(other._body)}
	{ ETC_TRACE_CTOR("by stealing", other); }

	Request::~Request()
	{ ETC_TRACE_DTOR(); }

	Request& Request::url(std::string value)
	{
		if (value.empty())
			value.push_back('/');
		else if (value[0] != '/')
			value = "/" + value;
		_url = std::move(value);
		ETC_LOG.debug(*this, "has now url", _url);
		return *this;
	}

	namespace {

		ETC_TEST_CASE(ctor)
		{
			{ Request r; ETC_ENFORCE_EQ(r.url(), "/"); }
			{ auto r = Request();  ETC_ENFORCE_EQ(r.url(), "/"); }
			{ auto r = Request().url("/pif"); ETC_ENFORCE_EQ(r.url(), "/pif"); }
			{ ETC_ENFORCE_EQ(Request().url("/pif").url(), "/pif"); }
			{ Request r = Request().url("/pif"); ETC_ENFORCE_EQ(r.url(), "/pif");}
			{ Request r;  r.url("/pif"); ETC_ENFORCE_EQ(r.url(), "/pif");}
		}

	} // !anonymous

}}
