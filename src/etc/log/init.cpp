#include "init.hpp"

#include <etc/exception.hpp>
#include <etc/log.hpp>

namespace etc { namespace log {

	ETC_LOG_COMPONENT("etc.log");

	Init::Init()
	{ ETC_TRACE_CTOR(); }

	Init::~Init()
	{
		ETC_TRACE_DTOR();
		try { shutdown(); }
		catch (...) {
			std::cerr << "Couldn't shutdown the logger: "
			          << exception::string() << std::endl;
		}
	}

}}

