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
# if ((defined(__GNUC__) && __GNUC__ == 4 &&  __GNUC_MINOR__ <= 7 ) && !defined(__clang__)) || \
			defined(BOOST_MSVC) || defined(__clang__)
			static boost::thread_specific_ptr<Indent> indent;
			if (indent.get() == nullptr)
				indent.reset(new Indent);
			return *indent;
#else
			static thread_local Indent value;
			return value;
#endif
		}

	} // !anonymous

	Log::Log(Level const level,
	         char const* file,
	         size_type const line,
	         char const* function,
	         Component const& component) ETC_NOEXCEPT
		: _line{level, file, line, function, component, 1}
		, _message{}
		, _sent{false}
	{}

	Log::Log(Log&& other) ETC_NOEXCEPT
		: _line{std::move(other._line)}
		, _message{std::move(other._message)}
		, _sent{other._sent}
	{
		if (!_sent && !_message.empty() && _should_log())
		{
			_line.indent = indent().increment();
			_logger().message(std::move(_line), std::move(_message));
			_sent = true;
		}
		other._sent = false;
	}

	Log::~Log()
	{
		if (!_sent && !_message.empty() && _should_log())
		{
			_logger().message(std::move(_line), std::move(_message));
		}
		else if (_sent)
			indent().decrement();
	}

	bool Log::_should_log() ETC_NOEXCEPT
	{
		return _logger().should_log(_line);
	}
}}
