#ifndef  ETC_NETWORK_SOCKETIMPL_HPP
# define ETC_NETWORK_SOCKETIMPL_HPP

# include "Socket.hpp"

# include <etc/types.hpp>

namespace etc { namespace network {

	struct Socket::Impl
	{
		virtual Socket::buffer_type read(etc::size_type s) = 0;
		virtual size_t write(Socket::buffer_type const& data) = 0;
		virtual ~Impl();
	};

}}

#endif

