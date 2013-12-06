#ifndef  ETC_ABORT_HPP
# define ETC_ABORT_HPP

# include "api.hpp"
# include "compiler.hpp"

# include <string>

namespace etc {

	class ETC_API_EXCEPTION AbortError
	{
	private:
		std::string _message;
	public:
		AbortError(std::string message) ETC_NOEXCEPT;
		virtual ~AbortError();
		virtual std::string const& what() const ETC_NOEXCEPT;
	};


	/// Throw an abort error.
	ETC_API void abort(std::string message = "");

}

#endif
