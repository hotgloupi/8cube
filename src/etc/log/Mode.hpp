#ifndef  ETC_LOG_MODE_HPP
# define ETC_LOG_MODE_HPP

# include <etc/compiler.hpp>

# include <iosfwd>

namespace etc { namespace log {

	enum class Mode
	{
		synchroneous = 0,
		asynchroneous = 1
	};

	/**
	 * Default logging mode.
	 *
	 * Check for env var ETC_LOG_SYNC or ETC_LOG_ASYNC, or returns true.
	 */
	Mode default_mode() ETC_NOEXCEPT;

	std::ostream& operator <<(std::ostream& out, Mode const mode);

}}

#endif
