#ifndef  ETC_ABORT_HPP
# define ETC_ABORT_HPP

# include <string>

# include <etc/api.hpp>

namespace etc {

	class ETC_API_EXCEPTION AbortError
	{
	private:
		std::string _message;
	public:
		AbortError(std::string message) noexcept;
		virtual ~AbortError();
		virtual std::string const& what() const noexcept;
	};


	/// Throw an abort error.
	ETC_API void abort(std::string message = "");

}

#endif
