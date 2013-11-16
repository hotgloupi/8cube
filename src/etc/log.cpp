#include "log.hpp"

#include <boost/thread/tss.hpp>

namespace etc { namespace log {

	namespace {

		struct Indent
		{
		private:
			size_type _indent;

		public:
			inline
			Indent() noexcept
				: _indent{0}
			{}

			inline
			size_type increment() noexcept
			{ return ++_indent; }

			inline
			size_type decrement() noexcept
			{
				if (_indent > 0)
					return --_indent;
				assert(false && "Cannot decrement indentation");
				return 0;
			}

			inline
			size_type get() noexcept
			{ return _indent; }
		};

		Indent& indent()
		{
			static boost::thread_specific_ptr<Indent> indent;
			if (indent.get() == nullptr)
				indent.reset(new Indent);
			return *indent;
		}

	} // !anonymous

	Log::Log(Level const level,
	         char const* file,
	         size_type const line,
	         char const* function,
	         std::string const& component) noexcept
		: _line{level, file, line, function, component, indent().increment()}
		, _logger{&logger(component)}
		, _should_log{_logger->should_log(_line)}
		, _message{}
	{}

	Log::Log(Log&& other) noexcept
		: _line{std::move(other._line)}
		, _logger{other._logger}
		, _should_log{other._should_log}
		, _message{std::move(other._message)}
	{ other._logger = nullptr; }

	Log::~Log()
	{
		if (_logger != nullptr)
		{
			if (_should_log)
				_logger->message(std::move(_line), std::move(_message));
			indent().decrement();
		}
	}

}}
