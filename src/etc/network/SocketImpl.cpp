#include "SocketImpl.hpp"

#include <etc/exception.hpp>

namespace etc { namespace network {

	Socket::buffer_type Socket::Impl::read(etc::size_type s)
	{ throw exception::Exception{"Invalid operation"}; }

	size_t Socket::Impl::write(Socket::buffer_type const& data)
	{ throw exception::Exception{"Invalid operation"}; }

	void Socket::Impl::bind(std::string address, uint16_t const port)
	{ throw exception::Exception{"Invalid operation"}; }

	void Socket::Impl::listen()
	{ throw exception::Exception{"Invalid operation"}; }

	Socket Socket::Impl::accept()
	{ throw exception::Exception{"Invalid operation"}; }

}}
