#include "SocketImpl.hpp"

#include <etc/log.hpp>
#include <etc/scheduler/SchedulerImpl.hpp>
#include <etc/test.hpp>

#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/read.hpp>

namespace etc { namespace network {

	using exception::Exception;

	ETC_LOG_COMPONENT("etc.network.Socket");

	Socket::Impl::~Impl()
	{}

	namespace {

		template<typename AsioSocket>
		struct BasicSocketImpl
			: public Socket::Impl
		{
			typedef typename AsioSocket::endpoint_type endpoint_type;
			typedef typename AsioSocket::protocol_type protocol_type;

			scheduler::Scheduler& sched;
			AsioSocket asio_socket;


			BasicSocketImpl(scheduler::Scheduler& sched)
				: sched(sched)
				, asio_socket{sched.impl().service}
			{}

			void bind(std::string address, uint16_t const port) override
			{
				ETC_TRACE.debug(*this, "Binding to", address, port);
				namespace ip = boost::asio::ip;
				if (address == "0")
					address = "0.0.0.0";
				auto addr = ip::address::from_string(address.c_str());
				ETC_LOG.debug(*this, "built address object:", addr);
				if (!this->asio_socket.is_open())
				{
					if (addr.is_v4())
						this->asio_socket.open(protocol_type::v4());
					else if (addr.is_v6())
						this->asio_socket.open(protocol_type::v6());
					else
						throw exception::Exception{"Unsupported address protocol"};
				}
				this->asio_socket.bind(endpoint_type(addr, port));
			}
		};

		template<typename AsioSocket>
		struct ClientSocketImpl
			: public BasicSocketImpl<AsioSocket>
		{
			typedef BasicSocketImpl<AsioSocket> Super;
			ClientSocketImpl(scheduler::Scheduler& sched)
				: Super{sched}
			{}

			Socket::buffer_type read(etc::size_type size) override
			{
				Socket::buffer_type res;
				auto ctx = this->sched.current();
				if (ctx != nullptr)
				{
					this->asio_socket.async_receive(
						boost::asio::buffer(res),
						0,
						//boost::asio::transfer_exactly(size),
						[&] (boost::system::error_code const& ec,
						     std::size_t bytes_transferred)
						{
							if (ec)
								ctx->exception = std::make_exception_ptr(
									Exception{"Read error:"}
								);
							this->sched.impl().wakeup(*ctx);
						}
					);
					this->sched.impl().freeze(*ctx);
					ctx->yield();
				}
				return res;
			}

			size_t write(Socket::buffer_type const& data) override
			{
				return 0;
			}
		};

		template<typename AsioSocket>
		struct AcceptSocketImpl
			: public BasicSocketImpl<AsioSocket>
		{
			typedef BasicSocketImpl<AsioSocket> Super;
			AcceptSocketImpl(scheduler::Scheduler& sched)
				: Super{sched}
			{}

			void listen() override
			{}

			Socket accept() override
			{ throw false; }
		};

		typedef ClientSocketImpl<boost::asio::ip::tcp::socket> ClientTCPSocket;
		typedef ClientSocketImpl<boost::asio::ip::udp::socket> ClientUDPSocket;
		typedef AcceptSocketImpl<boost::asio::ip::tcp::acceptor> AcceptTCPSocket;

	} // !anonymous

	Socket::Socket(int config,
	               scheduler::Scheduler* sched)
		: config{config}
	{
		ETC_TRACE_CTOR("with config", config);
		auto& s = sched == nullptr ? scheduler::current() : *sched;
		if (config & client_mode)
		{
			if (config & tcp_protocol)
				_this.reset(new ClientTCPSocket{s});
			else if (config & udp_protocol)
				_this.reset(new ClientUDPSocket{s});
		}
		else if (config & accept_mode)
		{
			if (config & tcp_protocol)
				_this.reset(new AcceptTCPSocket{s});
		}
		if (_this == nullptr)
			throw exception::Exception{"No implem found for this config"};
	}

	Socket::Socket(Socket&& other)
		: config{other.config}
		, _this{std::move(other._this)}
	{ ETC_TRACE_CTOR("by stealing", other); }

	Socket::~Socket()
	{ ETC_TRACE_DTOR(); }

	auto Socket::read(int size) -> buffer_type
	{ return _this->read(size > 0 ? size : 0); }

	size_t Socket::write(buffer_type const& data)
	{ return _this->write(data); }

	void Socket::bind(std::string address, uint16_t const port)
	{
		_this->bind(std::move(address), port);
	}

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
			scheduler::Scheduler _sched;
			void run_case()
			{
				_sched.spawn(
					"run test case " + _case->name,
					[=] (scheduler::Context&) { CaseSetupBase::run_case();}
				);
				_sched.run();
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
					{ Socket s(tcp_client); s.bind(addr, port); }
					{ Socket s(tcp_accept); s.bind(addr, port); }
					{ Socket s(udp_client); s.bind(addr, port); }
				}
		}

	}

}}
