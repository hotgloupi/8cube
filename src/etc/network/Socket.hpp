#ifndef  ETC_NETWORK_SOCKET_HPP
# define ETC_NETWORK_SOCKET_HPP

# include <etc/scheduler.hpp>

# include <memory>
# include <vector>

namespace etc { namespace network {

	class Socket
	{
	public:
		typedef std::vector<uint8_t> buffer_type;

	public:
		explicit Socket(scheduler::Scheduler* sched = nullptr);
		~Socket();

		buffer_type read(int size = 0);
		size_t write(buffer_type const& data);

	public:
		struct Impl;
	private:
		std::unique_ptr<Impl> _this;
	};

}}

#endif
