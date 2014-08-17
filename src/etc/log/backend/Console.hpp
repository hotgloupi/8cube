#ifndef  ETC_LOG_BACKEND_CONSOLE_HPP
# define ETC_LOG_BACKEND_CONSOLE_HPP

# include "Interface.hpp"

namespace etc { namespace log { namespace backend {

	class Console
		: public Interface
	{
	private:
		struct Impl;
		std::unique_ptr<Impl> _this;

	public:
		Console();
		~Console();
		void _send_line(Line line, std::string message) ETC_NOEXCEPT override;
	};

}}}

#endif
