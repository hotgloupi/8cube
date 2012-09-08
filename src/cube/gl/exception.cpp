#include "exception.hpp"

#include <etc/backtrace.hpp>
#include <etc/log.hpp>

namespace cube { namespace gl { namespace exception {

	ETC_LOG_COMPONENT("cube.gl.exception.Exception");

	Exception::Exception(std::string const& msg)
		: cube::exception::Exception(msg)
	{
		auto error = ETC_LOG.error("Exception thrown:", msg);
		if (auto bt = this->backtrace())
		{
			etc::size_type i = 0;
			for (auto const& frame: *bt)
				error("\t", i++, frame);
		}
	}

}}}
