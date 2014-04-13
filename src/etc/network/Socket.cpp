#include "SocketImpl.hpp"

#include "Exception.hpp"

#include <etc/log.hpp>
#include <etc/scheduler/SchedulerImpl.hpp>
#include <etc/test.hpp>

#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/read.hpp>
#include <boost/mpl/inherit.hpp>

namespace ip = boost::asio::ip;

namespace etc { namespace network {

	using exception::Exception;

	ETC_LOG_COMPONENT("etc.network.Socket");

	namespace {

		//
		// What you see below is a twisted way to make mixin classes that
		// implement the Socket::Impl interface.
		//
		// the class SocketImpl will merge specified operations and forward
		// calls to the mixin. This way, we can define any kind of sockets by
		// just mixing needed operations. See the typedefs ClientTCPSocket and
		// friends. Note that methods cannot be overriden between operations
		// thanks to boost::mpl::inherit<> magic.
		//

		// Declare a socket operation.
#define DECL_OPERATION(__class) \
		template<typename AsioSocket, typename Self> \
		struct __class \
/**/

		// Define an operation class.
#define DEF_OPERATION(__class) \
		DECL_OPERATION(__class) \
			: public virtual Socket::Impl
/**/

		// Exports typedefs and self() in the Operation class.
#define PATCH_OPERATION() \
			Self& self() { return static_cast<Self&>(*this); } \
			typedef typename AsioSocket::protocol_type protocol_type; \
			typedef typename AsioSocket::endpoint_type endpoint_type; \
			typedef AsioSocket asio_socket_type; \
/**/

		DEF_OPERATION(Bind)
		{
			PATCH_OPERATION();

			void bind(std::string const& address, uint16_t const port) override
			{
				ETC_TRACE.debug(self(), "Binding to", address, port);
				namespace ip = boost::asio::ip;
				ip::address addr = self().make_address(address);
				ETC_LOG.debug(self(), "built address object:", addr);
				if (!self().asio_socket.is_open())
					self().open_for(addr);
				self().asio_socket.bind(endpoint_type(addr, port));
			}
		};

		DEF_OPERATION(Connect)
		{
			PATCH_OPERATION();

			void connect(std::string const& address, uint16_t port) override
			{
				ETC_LOG.debug(*this, "Connecting to", address, port);
				auto& ctx = self().sched.context();
				auto addr = self().make_address(address);
				endpoint_type ep{addr, port};
				self().asio_socket.async_connect(
					ep,
					[&] (boost::system::error_code const& ec) {
						if (ec)
							ctx.exception = self().make_exception(
								ec, "Couldn't connect to " + etc::to_string(ep)
							);
						self().sched.impl().wakeup(ctx);
					}
				);
				self().sched.impl().freeze(ctx);
				ctx.yield();
			}
		};

		DEF_OPERATION(ReadWrite)
		{
			PATCH_OPERATION();

			Socket::buffer_type read(etc::size_type size) override
			{
				if (size == 0)
					ETC_LOG.debug(self(), "Receiving bytes until connection close");
				else
					ETC_LOG.debug(self(), "Receiving", size, "bytes");
				Socket::buffer_type res;
				auto& ctx = self().sched.context();
				std::size_t read_bytes = 0;
				do
				{
					size_t to_read = size == 0 ? 4096 : size - read_bytes;
					if (res.size() < read_bytes + to_read)
						res.resize(read_bytes + to_read);
					self().asio_socket.async_receive(
						boost::asio::buffer(&res[read_bytes], to_read),
						0,
						[&] (boost::system::error_code const& ec,
							 std::size_t bytes_transferred)
						{
							if (ec)
								ctx.exception = self().make_exception(
									ec, "Couldn't read"
								);
							else if (bytes_transferred == 0)
								ctx.exception = std::make_exception_ptr(
									Exception{"Invalid state ?"}
								);
							else
							{
								read_bytes += bytes_transferred;
								ETC_LOG.debug(self(), "Got", bytes_transferred,
								              "/", to_read, "bytes");
							}
							self().sched.impl().wakeup(ctx);
						}
					);
					self().sched.impl().freeze(ctx);
					ctx.yield();
				} while (size == 0 || read_bytes < size);
				ETC_LOG.debug(self(), "Finalize read of", read_bytes, "/",
				              size == 0 ? "(until EOF)" : std::to_string(size),
				              "bytes");
				res.resize(read_bytes);
				return res;
			}

			size_t write(Socket::buffer_type const& data) override
			{
				auto& ctx = self().sched.context();
				size_t sent = 0;
				while (sent < data.size())
				{
					ETC_LOG.debug(self(), "Sending", data.size() - sent, "/",
					              data.size(), "bytes");
					self().asio_socket.async_write_some(
						boost::asio::buffer(&data[sent], data.size() - sent),
						[&] (boost::system::error_code const& ec,
							 std::size_t bytes_transferred)
						{
							if (ec)
								ctx.exception = self().make_exception(
									ec, "Couldn't write"
								);
							else if (bytes_transferred == 0)
								ctx.exception = std::make_exception_ptr(
									Exception{"Invalid state"}
								);
							else
								sent += bytes_transferred;
							ETC_LOG.debug(self(), "Sent", bytes_transferred,
							              "bytes" ",", data.size() - sent,
							              "left");
							self().sched.impl().wakeup(ctx);
						}
					);
					self().sched.impl().freeze(ctx);
					ctx.yield();
				}
				ETC_LOG.debug("Finalized write of", sent, "/", data.size());
				return sent;
			}
		};

		DEF_OPERATION(Listen)
		{
			PATCH_OPERATION();

			void listen() override
			{
				ETC_LOG.debug(self(), "Listening");
				self().asio_socket.listen();
			}
		};

		// Helper to determine the accepted client socket type.
		template<typename AcceptorSocket> struct client_socket;

		DEF_OPERATION(Accept)
		{
			PATCH_OPERATION();
			typedef typename client_socket<asio_socket_type>::type client_socket_type;

			Socket accept() override
			{
				ETC_LOG.debug(self(), "Accepting incoming connection");
				auto& ctx = self().sched.context();
				auto impl = etc::make_unique<client_socket_type>(
					(self().config & ~_config_mode_mask) | config_client_mode,
					self().sched
				);
				self().asio_socket.async_accept(
					impl->asio_socket,
					[&] (boost::system::error_code const& ec) {
						if (ec)
							ctx.exception = self().make_exception(
								ec, "Couldn't accept incoming connection"
							);
						self().sched.impl().wakeup(ctx);
					}
				);
				self().sched.impl().freeze(ctx);
				ctx.yield();
				return Socket(std::unique_ptr<Socket::Impl>(impl.release()));
			}
		};


		template<typename AsioSocket, template <typename /*socket*/, typename /*self*/> class... Operations>
		struct SocketImpl
			: public boost::mpl::inherit<
				Operations<AsioSocket, SocketImpl<AsioSocket, Operations...>>...
			  >::type
			, public virtual Socket::Impl
		{
			typedef typename boost::mpl::inherit<
				Operations<AsioSocket, SocketImpl<AsioSocket, Operations...>>...
			  >::type mixin;
			typedef typename AsioSocket::endpoint_type endpoint_type;
			typedef typename AsioSocket::protocol_type protocol_type;
			typedef AsioSocket asio_socket_type;
			typedef SocketImpl<AsioSocket, Operations...> self_type;

			int config;
			scheduler::Scheduler& sched;
			AsioSocket asio_socket;

			SocketImpl(int config, scheduler::Scheduler& sched)
				: Socket::Impl{}
				, config{config}
				, sched(sched)
				, asio_socket{sched.impl().service}
			{}

			int configuration() const override
			{ return this->config; }

			Socket::buffer_type read(etc::size_type s) override
			{ return mixin::read(s); }

			size_t write(Socket::buffer_type const& data) override
			{ return mixin::write(data); }

			void bind(std::string const& address, uint16_t const port) override
			{ return mixin::bind(address, port); }

			void listen() override
			{ return mixin::listen(); }

			Socket accept() override
			{ return mixin::accept(); }

			static
			ip::address make_address(std::string const& str)
			{
				if (str == "0")
					return ip::address::from_string("0.0.0.0");
				return ip::address::from_string(str.c_str());
			}

			static
			std::exception_ptr make_exception(boost::system::error_code const& ec,
			                                  std::string message)
			{
				return std::make_exception_ptr(
					SystemError{
						message,
						std::error_code(ec.value(), std::system_category())
					}
				);
			}

			void open_for(ip::address const& addr)
			{
				ETC_TRACE.debug(*this, "Opening socket for", addr);
				if (this->asio_socket.is_open())
					throw Exception{"Socket already opened"};
				if (addr.is_v4())
					this->asio_socket.open(protocol_type::v4());
				else if (addr.is_v6())
					this->asio_socket.open(protocol_type::v6());
				else
					throw Exception{"Unsupported address protocol"};
				if (this->config & config_reuse_address)
				{
					ETC_LOG.debug(*this, "Enable reuse address option");
					this->asio_socket.set_option(
						typename AsioSocket::reuse_address(true)
					);
				}
			}
		};

		typedef SocketImpl<ip::tcp::socket, Bind, Connect, ReadWrite> ClientTCPSocket;
		//typedef SocketImpl<ip::udp::socket, Bind, ReadWrite> ClientUDPSocket;

		template<> struct client_socket<ip::tcp::acceptor>
		{ typedef ClientTCPSocket type; };
		typedef SocketImpl<ip::tcp::acceptor, Bind, Listen, Accept> AcceptTCPSocket;

	} // !anonymous

	Socket::Socket(int config,
	               scheduler::Scheduler* sched)
	{
		ETC_TRACE_CTOR("with config", config);
		auto& s = sched == nullptr ? scheduler::current() : *sched;
		if (config & config_client_mode)
		{
			if (config & config_tcp_protocol)
				_this.reset(new ClientTCPSocket{config, s});
		//	else if (config & config_udp_protocol)
		//		_this.reset(new ClientUDPSocket{config, s});
		}
		else if (config & config_accept_mode)
		{
			if (config & config_tcp_protocol)
				_this.reset(new AcceptTCPSocket{config, s});
		}
		if (_this == nullptr)
			throw exception::Exception{"No implem found for this config"};
	}

	Socket::Socket(Socket&& other)
		: _this{std::move(other._this)}
	{ ETC_TRACE_CTOR("by stealing", other); }

	Socket::Socket(std::unique_ptr<Impl> impl)
		:_this{std::move(impl)}
	{ ETC_TRACE_CTOR("internally with config", _this->configuration()); }

	Socket::~Socket()
	{ ETC_TRACE_DTOR(); }

	auto Socket::read(int size) -> buffer_type
	{ return _this->read(size > 0 ? size : 0); }

	size_t Socket::write(buffer_type const& data)
	{ return _this->write(data); }

	void Socket::listen()
	{ return _this->listen(); }

	void Socket::bind(std::string const& address, uint16_t const port)
	{ _this->bind(address, port); }

	void Socket::connect(std::string const& address, uint16_t const port)
	{ _this->connect(address, port); }

	Socket Socket::accept()
	{ return _this->accept(); }

	namespace {

		ETC_TEST_CASE(ctor_no_scheduler)
		{
			ETC_TEST_THROW({
				Socket s;
			}, exception::Exception, "No scheduler available");
		}

		ETC_TEST_CASE(ctor_with_scheduler)
		{
			scheduler::Scheduler s;
			s.spawn("ctor_no_scheduler", [] (scheduler::Context&) { Socket s; });
			s.run();
		}

		// Run tests in a scheduler
		ETC_TEST_CASE_SETUP(WithScheduler)
		{
			std::unique_ptr<scheduler::Scheduler> _sched;
			void setUp() { _sched.reset(new scheduler::Scheduler()); }
			void tearDown() { _sched.reset(); }
			void run_case()
			{
				_sched->spawn(
					"run test case " + _case->name,
					[=] (scheduler::Context&) { CaseSetupBase::run_case();}
				);
				_sched->run();
			}
		};

#define SCHED_TEST_CASE(name) ETC_TEST_CASE_WITH(name, WithScheduler)

		SCHED_TEST_CASE(bind)
		{
			std::vector<std::string> addresses = {
				"127.0.0.1",
				"0.0.0.0",
				"0",
			};
			std::vector<uint16_t> ports = {
				0, 8080
			};
			for (auto const& addr: addresses)
				for (auto port: ports)
				{
					{ Socket s(config_tcp_client); s.bind(addr, port); }
					{ Socket s(config_tcp_accept); s.bind(addr, port); }
			//		{ Socket s(config_udp_client); s.bind(addr, port); }
				}
		}

		SCHED_TEST_CASE(connect)
		{
			Socket server(config_tcp_accept);
			server.bind("127.0.0.1", 12345);
			server.listen();

			scheduler::spawn("client", [&] {
				Socket s;
				s.connect("127.0.0.1", 12345);
				ETC_TEST_EQ(s.read(4), "POUF");
			});
			auto client = server.accept();
			client.write("POUF");
		}

	}

}}
