#include "init.hpp"

#include <etc/exception.hpp>
#include <etc/log.hpp>

#include <curl/curl.h>

namespace etc { namespace http {

	ETC_LOG_COMPONENT("etc.http");

	Init::Init()
	{
		ETC_TRACE_CTOR();
		if (::curl_global_init(CURL_GLOBAL_ALL) != 0)
			throw exception::Exception{"Cannot initialize etc.http"};
	}

	Init::~Init()
	{
		ETC_TRACE_DTOR();
		curl_global_cleanup();
	}

}}


