#include "abort.hpp"

#include <etc/log.hpp>

namespace etc {

	ETC_LOG_COMPONENT("etc.AbortError");

	AbortError::AbortError(std::string message) ETC_NOEXCEPT
		: _message{std::move(message)}
	{ ETC_LOG.fatal(message); }

	AbortError::~AbortError()
	{}

	std::string const& AbortError::what() const ETC_NOEXCEPT
	{ return _message; }


	void abort(std::string message)
	{ throw AbortError{std::move(message)}; }

}
