#include "SocketImpl.hpp"
#include "Exception.hpp"

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

}}
