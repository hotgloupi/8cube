#ifndef  ETC_NETWORK_SOCKET_HPP
# define ETC_NETWORK_SOCKET_HPP

# include <etc/scheduler.hpp>

# include <memory>
# include <vector>

namespace etc { namespace network {

	enum {
		config_tcp_protocol  = 1 << 0,
		config_udp_protocol  = 1 << 1,
		config_client_mode   = 1 << 2,
		config_accept_mode   = 1 << 3,
		config_reuse_address = 1 << 4,

		config_tcp_client = (config_tcp_protocol | config_client_mode),
		config_tcp_accept = (config_tcp_protocol | config_accept_mode | config_reuse_address),
		config_udp_client = (config_udp_protocol | config_client_mode),
		_config_mode_mask = (config_client_mode | config_accept_mode),
		_config_protocol_mask = (config_tcp_protocol | config_udp_protocol),

		// Not fragmented UDP packet size
		// (MTU min size - IP header size - UDP header size)
		safe_udp_size = 576 - 60 - 8,

		// Almost never fragmented UDP packet size
		// (MTU avg size - IP header size - UDP header size)
		best_udp_size = 1500 - 60 - 8,

		// Maximum UDP packet size
		// (UPD max size - IP header min size - UPD header size)
		max_udp_size = 65535 - 20 - 8,

	};

	class Socket
	{
	public:
		typedef std::string buffer_type;
		struct Impl;

	private:
		std::unique_ptr<Impl> _this;

	public:
		explicit
		Socket(int config = config_tcp_client,
		       scheduler::Scheduler* sched = nullptr);
		Socket(Socket&& other);
		~Socket();
		Socket(std::unique_ptr<Impl> impl);

	public:
		/**
		 * Read from the socket until size bytes have been read (or until the
		 * end of time if size <= 0).
		 *
		 * Notes for TCP sockets:
		 *   If the size of the returned buffer returned is less than the one
		 *   asked, any subsequent calls to this method will fail, unless the
		 *   socket is reconnected to a valid peer.
		 *
		 * Notes for UDP sockets:
		 *   Specifying a size (!= 0) is an error.
		 */
		buffer_type read(int size = 0);
		size_t write(buffer_type const& data);

		void bind(std::string const& address, uint16_t const port);
		void connect(std::string const& address, uint16_t const port);
		void listen();
		Socket accept();
	};

}}

#endif
