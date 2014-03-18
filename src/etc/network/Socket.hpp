#ifndef  ETC_NETWORK_SOCKET_HPP
# define ETC_NETWORK_SOCKET_HPP

# include <etc/scheduler.hpp>

# include <memory>
# include <vector>

namespace etc { namespace network {

	enum {
		tcp_protocol = 1 << 0,
		udp_protocol = 1 << 1,

		client_mode = 1 << 2,
		accept_mode = 1 << 3,

		tcp_client = (tcp_protocol | client_mode),
		tcp_accept = (tcp_protocol | accept_mode),
		udp_client = (udp_protocol | client_mode),
	};

	class Socket
	{
	public:
		typedef std::vector<uint8_t> buffer_type;
		struct Impl;

	public:
		int const config;
	private:
		std::unique_ptr<Impl> _this;

	public:
		explicit
		Socket(int config = tcp_client,
		       scheduler::Scheduler* sched = nullptr);
		Socket(Socket&& other);
		~Socket();

		buffer_type read(int size = 0);
		size_t write(buffer_type const& data);

		void bind(std::string address, uint16_t const port);
		void listen();
	};

}}

#endif
