#ifndef  ETC_HTTP_RESPONSE_HPP
# define ETC_HTTP_RESPONSE_HPP

# include "fwd.hpp"

# include <memory>
# include <iosfwd>

namespace etc { namespace http {

	class Response
	{
	public:
		struct Impl;

	public:
		Response(std::unique_ptr<Impl> impl);
		Response(Response&& other);
		~Response();

	public:
		ResponseCode code();
		std::istream& body();

	private:
		std::unique_ptr<Impl> _this;
	public:
		Impl& impl() { return *_this; }
	};

}}

#endif
