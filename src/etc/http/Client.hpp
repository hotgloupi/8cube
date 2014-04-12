#ifndef  ETC_HTTP_CLIENT_HPP
# define ETC_HTTP_CLIENT_HPP

# include "fwd.hpp"

# include <etc/api.hpp>
# include <etc/types.hpp>
# include <etc/scheduler.hpp>

# include <memory>

namespace etc { namespace http {

	class ETC_API Client
	{
	public:
		Client(std::string server,
		       uint16_t port = 80,
		       scheduler::Scheduler* sched = nullptr);
		~Client();

		std::string const& server() const ETC_NOEXCEPT;
		uint16_t port() const ETC_NOEXCEPT;

		Response fire(Request req);

	public:
		struct Impl;
	private:
		std::unique_ptr<Impl> _this;
	public:
		Impl& impl() { return *_this; }
	};

}}

#endif
