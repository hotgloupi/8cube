#include "init.hpp"

#include <etc/exception.hpp>
#include <etc/log.hpp>
#include <etc/log/Logger.hpp>
#include <etc/log/detail/log.hpp>

namespace etc { namespace log {

	ETC_LOG_COMPONENT("etc.log");

	Init::Init()
	{ ETC_TRACE_CTOR(); }

	Init::~Init()
	{
		ETC_TRACE_DTOR();
		try { shutdown(); }
		catch (...) {
			detail::log(
			 	"Couldn't shutdown the logger:",
			 	exception::string()
			);
		}
	}

}}

