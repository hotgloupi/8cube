#include "assert.hpp"

namespace etc {

	AssertError::AssertError(std::string message) noexcept
		: _message{std::move(message)}
	{}

	AssertError::~AssertError()
	{}

	std::string const& AssertError::what() const noexcept
	{ return _message; }

	namespace detail {

		void raise_assert(char const* file,
		                  unsigned int line,
		                  std::string message)
		{
			throw AssertError{
				std::string(file) + ":" + std::to_string(line) + ": " +
				std::move(message)
			};
		}

	} // !detail

}
