#ifndef  ETC_ABORT_HPP
# define ETC_ABORT_HPP

# include <string>

namespace etc {

	class AbortError
	{
	private:
		std::string _message;
	public:
		AbortError(std::string message) noexcept;
		virtual ~AbortError();
		virtual std::string const& what() const noexcept;
	};


	/// Throw an abort error.
	void abort(std::string message = "");

}

#endif
