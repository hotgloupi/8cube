#include "exception.hpp"

#include "backtrace.hpp"

#include <etc/log.hpp>
#include <etc/sys/environ.hpp>

#include <exception>
#include <iostream>

namespace etc { namespace exception {

	ETC_LOG_COMPONENT("etc.exception.Exception");

	std::string string()
	{ return string(std::current_exception()); }

	std::string string(std::exception_ptr e)
	{
		try { std::rethrow_exception(e); }
		catch (std::exception const& err)
		{ return err.what(); }
		catch (...)
		{ return "Unknown error type"; }
	}

	Exception::Exception(std::string const& msg)
		: std::runtime_error(msg)
		, _backtrace(nullptr)
	{
		try
		{
			_backtrace = new backtrace::Backtrace();
		}
		catch (std::exception const& err)
		{
			try { _msg = std::string(err.what()); }
			catch (std::exception const&) {}
		}
		static bool const logging_exception =
			etc::sys::environ::try_as<bool>("ETC_LOG_EXCEPTION");
		if (logging_exception)
			ETC_LOG.warn("Raising:", *this);
	}

	Exception::~Exception() throw ()
	{
		delete _backtrace;
		_backtrace = nullptr;
	}

	std::ostream& operator <<(std::ostream& out, Exception const& e)
	{
		out << e.what() << ":";
		if (e._backtrace == nullptr)
		{
			out << "No backtrace available: ";
			if (e._msg.size() == 0)
				out << "Couldn't allocate the reason message !";
			else
				out << e._msg;
		}
		else
			out << e._msg << ":\n" <<  *e._backtrace;
		return out;
	}

}}
