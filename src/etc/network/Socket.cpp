#include "SocketImpl.hpp"

#include <etc/log.hpp>
#include <etc/scheduler/SchedulerImpl.hpp>

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
		struct SocketImpl
			: public Socket::Impl
		{
			scheduler::Scheduler& sched;
			AsioSocket asio_socket;


			SocketImpl(scheduler::Scheduler& sched)
				: sched(sched)
				, asio_socket{sched.impl().service}
			{}

			Socket::buffer_type read(etc::size_type size) override
			{
				Socket::buffer_type res;
				auto ctx = this->sched.current();
				if (ctx != nullptr)
				{
					boost::asio::async_read(
						this->asio_socket,
						boost::asio::buffer(res),
						boost::asio::transfer_exactly(size),
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

		typedef SocketImpl<boost::asio::ip::tcp::socket> TCPSocket;
		typedef SocketImpl<boost::asio::ip::udp::socket> UDPSocket;

	} // !anonymous

	Socket::Socket(scheduler::Scheduler* sched)
		: _this{new TCPSocket{sched == nullptr ? *scheduler::current() : *sched}}
	{ ETC_TRACE_CTOR(); }

	Socket::~Socket()
	{ ETC_TRACE_CTOR(); }

	auto Socket::read(int size) -> buffer_type
	{ return _this->read(size > 0 ? size : 0); }

	size_t Socket::write(buffer_type const& data)
	{ return _this->write(data); }

}}
