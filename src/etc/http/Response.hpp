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
		Response(std::shared_ptr<Impl> impl);
		Response(Response&& other);
		~Response();

	public:
		ResponseCode code();
		std::string read(size_t size = 0);
		//std::istream& body();

	private:
		std::shared_ptr<Impl> _this;
	public:
		Impl& impl() { return *_this; }
	};

}}

#endif
