#include "log.hpp"

namespace etc { namespace log {


	// static variable indent
	namespace {

		struct Indent
		{
			private:
				static unsigned int _indent;
			public:
				static inline unsigned int increment() { return ++_indent; }
				static inline unsigned int decrement() { return --_indent; }
				static inline unsigned int get() { return _indent; }
		} indent;


		unsigned int Indent::_indent = 0;

	} // !anonymous

	Log::Log(Level level,
	         std::string const& file,
	         unsigned int line,
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
