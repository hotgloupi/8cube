#ifndef  ETC_SCHEDULER_STRANDIMPL_HPP
# define ETC_SCHEDULER_STRANDIMPL_HPP

# include "Strand.hpp"

# include <boost/asio/strand.hpp>

namespace etc { namespace scheduler {

	struct Strand::Impl
	{
		boost::asio::strand strand;

		Impl(boost::asio::io_service& service)
			: strand{service}
		{}
	};

}}

#endif
