#ifndef  ETC_LOG_DETAIL_RUNNER_HPP
# define ETC_LOG_DETAIL_RUNNER_HPP

# include <etc/log/Line.hpp>
# include <etc/log/fwd.hpp>

# include <memory>

namespace etc { namespace log { namespace detail {

	class Runner
	{
	private:
		struct Impl;
		std::unique_ptr<Impl> _this;

	public:
		Runner(backend::Interface& b);
		void post(Line line,
		          std::string message);
		void start();
		void stop();
		void shutdown();
		~Runner();
	};

}}}

#endif
