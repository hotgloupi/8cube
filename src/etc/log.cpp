#include "log.hpp"

namespace etc { namespace log {


	// static variable indent
	namespace {

		struct Indent
		{
			private:
				static size_type _indent;
			public:
				static inline size_type increment() { return ++_indent; }
				static inline size_type decrement() { return --_indent; }
				static inline size_type get() { return _indent; }
		} indent;


		size_type Indent::_indent = 0;

	} // !anonymous

	Log::Log(Level level,
	         std::string const& file,
	         size_type line,
	         std::string const& function,
	         std::string const& component)
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
		: _line{std::move(other._line)}
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
}}
