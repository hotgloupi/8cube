#include "log.hpp"

namespace etc { namespace log {


	// static variable indent
	//namespace {

		Indent indent;
		size_type Indent::_indent = 0;

	//} // !anonymous

	ETC_API Log::Log(Level level,
	                 char const* file,
	                 size_type line,
	                 char const* function,
	                 char const* component)
		: _line{
			level,
			file,
			line,
			function,
			component,
			indent.increment()
		}
		, _logger(logger(component))
		, _dtor_indent{true}
	{}

	Log::Log(Log const& o)
		: _line{o._line}
		, _logger(o._logger)
		, _dtor_indent{true}
	{
		_line.indent = indent.increment();
	}

	Log::Log(Log&& other)
		: _line(std::move(other._line))
		, _logger(other._logger)
		, _dtor_indent{true}
	{
		_line.indent = indent.increment();
		other._dtor_indent = false;
	}

	Log::~Log()
	{
		if (_dtor_indent)
			indent.decrement();
	}

	etc::size_type Log::_current_indent()
	{
		return indent.get();
	}
}}
