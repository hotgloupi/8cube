#include "assert.hpp"

#include <etc/log.hpp>

namespace etc {

	ETC_LOG_COMPONENT("etc");

	AssertError::AssertError(std::string message) ETC_NOEXCEPT
		: _message{std::move(message)}
	{}

	AssertError::~AssertError()
	{}

	std::string const& AssertError::what() const ETC_NOEXCEPT
	{ return _message; }

	namespace detail {

		void raise_assert(char const* file,
		                  unsigned int line,
		                  std::string message)
		{
			ETC_LOG.error(message);
			throw AssertError{
				std::string(file) + ":" + std::to_string(line) + ": " +
				std::move(message)
			};
		}

	} // !detail

}
