#ifndef  ETC_NETWORK_SOCKETIMPL_HPP
# define ETC_NETWORK_SOCKETIMPL_HPP

# include "Socket.hpp"

# include <etc/types.hpp>
# include <etc/printable.hpp>

namespace etc { namespace network {

	struct Socket::Impl
		: public Printable
	{
		virtual int configuration() const;
		virtual Socket::buffer_type read(etc::size_type s);
		virtual size_t write(Socket::buffer_type const& data);
		virtual void bind(std::string const& address, uint16_t const port);
		virtual void listen();
		virtual Socket accept();
		virtual void connect(std::string const& address, uint16_t port);
		void print(std::ostream& out) const ETC_NOEXCEPT override;
		virtual ~Impl();
	};

}}

#endif

