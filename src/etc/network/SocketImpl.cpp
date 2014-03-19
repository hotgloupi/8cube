#include "SocketImpl.hpp"
#include "Exception.hpp"

#include <etc/str.hpp>

namespace etc { namespace network {

	Socket::Impl::~Impl()
	{}

	Socket::buffer_type Socket::Impl::read(etc::size_type s)
	{ throw InvalidOperation{}; }

	size_t Socket::Impl::write(Socket::buffer_type const& data)
	{ throw InvalidOperation{}; }

	void Socket::Impl::bind(std::string const& address, uint16_t const port)
	{ throw InvalidOperation{}; }

	void Socket::Impl::listen()
	{ throw InvalidOperation{}; }

	Socket Socket::Impl::accept()
	{ throw InvalidOperation{}; }

	void Socket::Impl::connect(std::string const& address, uint16_t port)
	{ throw InvalidOperation{}; }

	void Socket::Impl::print(std::ostream& out) const ETC_NOEXCEPT
	{
		out << '<'
			<< str(ETC_TYPE_STRING(*this))
			.replace("Impl", "")
			.replace("etc::network::", "")
			.replace("(anonymous namespace)::", "")
			.replace("boost::asio::", "")
			.replace("basic_", "")
			.replace("socket_", "")
			.replace(", acceptor_service<ip::tcp> ", "")
			.replace(", stream_service<ip::tcp> ", "")
			.replace("> >", ">>")
			.replace("<", "(")
			.replace(">", ")")
			<< " at " << this << '>';
		;

	}

}}
