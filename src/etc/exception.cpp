#include "exception.hpp"

#include "backtrace.hpp"

#include <iostream>

namespace etc { namespace exception {

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
			try { _msg = std::string{err.what()}; }
			catch (std::exception const&) {}
		}
	}

	Exception::~Exception() throw ()
	{
		delete _backtrace;
		_backtrace = nullptr;
	}

	std::ostream& operator <<(std::ostream& out, Exception const& e)
	{
		out << e.what() << ":\n";
		if (e._backtrace == nullptr)
		{
			out << "No backtrace available: ";
			if (e._msg.size() == 0)
				out << "Couldn't allocate the reason message !";
			else
				out << e._msg;
		}
		else
			out << *e._backtrace;
		return out;
	}
}}
