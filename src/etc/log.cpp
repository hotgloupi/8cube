#include "log.hpp"
#include "log/Logger.hpp"

#include <boost/thread/tss.hpp>

namespace etc { namespace log {

	namespace {

		struct Indent
		{
		private:
			size_type _indent;

		public:
			inline
			Indent() ETC_NOEXCEPT
				: _indent{0}
			{}

			inline
			size_type increment() ETC_NOEXCEPT
			{ return ++_indent; }

			inline
			size_type decrement() ETC_NOEXCEPT
			{
				if (_indent > 0)
					return --_indent;
				assert(false && "Cannot decrement indentation");
				return 0;
			}

			inline
			size_type get() ETC_NOEXCEPT
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
	         std::string const& component) ETC_NOEXCEPT
		: _line{level, file, line, function, component, indent().increment()}
		, _logger{&logger(component)}
		, _message{}
		, _sent{false}
	{}

	Log::Log(Log&& other) ETC_NOEXCEPT
		: _line{std::move(other._line)}
		, _logger{other._logger}
		, _message{std::move(other._message)}
		, _sent{other._sent}
	{
		other._logger = nullptr;
		if (_should_log())
		{
			_sent = true;
			_logger->message(std::move(_line), std::move(_message));
		}
	}

	Log::~Log()
	{
		if (_logger != nullptr)
		{
			if (_should_log())
				_logger->message(std::move(_line), std::move(_message));
			indent().decrement();
		}
	}

	bool Log::_should_log() ETC_NOEXCEPT
	{
		return !_sent && _logger->should_log(_line);
	}
}}
