#include "exception.hpp"

#include "backtrace.hpp"

#include <etc/assert.hpp>
#include <etc/log.hpp>
#include <etc/sys/environ.hpp>
#include <etc/test.hpp>

#include <exception>
#include <iostream>

namespace etc { namespace exception {

	ETC_LOG_COMPONENT("etc.exception.Exception");

	std::string string()
	{ return string(std::current_exception()); }

	std::string string(std::exception_ptr e)
	{
		try { std::rethrow_exception(e); return "No exception thrown !"; }
		catch (std::exception const& err)
		{ return err.what(); }
		catch (etc::AssertError const& err)
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
			_backtrace.reset(new backtrace::Backtrace());
		}
		catch (std::exception const& err)
		{
			try { _msg += etc::to_string(" COULDN'T BUILD BACKTRACE:", err.what()); }
			catch (std::exception const&) {}
		}
		static bool const logging_exception =
			etc::sys::environ::try_as<bool>("ETC_LOG_EXCEPTION");
		if (logging_exception)
			ETC_LOG.warn("Raising:", *this);
	}

	Exception::Exception(Exception&& other)
		: std::runtime_error(other.what())
		, _backtrace(std::move(other._backtrace))
		, _msg(std::move(other._msg))
	{}

	Exception::Exception(Exception const& other)
		: std::runtime_error(other.what())
		, _backtrace{nullptr}
		, _msg(other._msg)
	{
		if (other._backtrace != nullptr)
			_backtrace.reset(new backtrace::Backtrace(*other._backtrace));
	}

	Exception::~Exception() ETC_NOEXCEPT
	{}

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

	KeyError::KeyError(std::string key)
		: Exception{"KeyError: " + key}
	{}

	ValueError::ValueError(std::string msg)
		: Exception{"ValueError: " + msg}
	{}

	namespace {

		ETC_TEST_CASE(make_exception_ptr)
		{
			auto e = std::make_exception_ptr(Exception{"Test"});
		}

	}

}}
