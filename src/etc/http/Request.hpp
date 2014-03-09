#ifndef  ETC_HTTP_REQUEST_HPP
# define ETC_HTTP_REQUEST_HPP

# include "fwd.hpp"

# include <etc/compiler.hpp>

# include <map>
# include <memory>

namespace etc { namespace http {

	class Request
	{
	public:
		typedef std::map<std::string, std::string> parameters_map;
		typedef std::map<std::string, std::string> headers_map;

	private:
		std::string    _url;
		parameters_map _parameters;
		Method         _method;
		headers_map    _headers;
		std::string    _body;

	public:
		Request() = default;
		Request(Request const& other) = default;
		Request(Request&& other) = default;
		~Request() {}

	public:
		std::string const& url() const ETC_NOEXCEPT
		{ return _url; }

		Request& url(std::string value)
		{ _url = std::move(value); return *this; }

	public:
		parameters_map const& parameters() const ETC_NOEXCEPT
		{ return _parameters; }

		parameters_map& parameters() ETC_NOEXCEPT
		{ return _parameters; }

		Request& parameter(std::string const& key, std::string value)
		{ _parameters[key] = std::move(value); return *this; }

	public:
		Method method() const ETC_NOEXCEPT
		{ return _method; }

		Request& method(Method value)
		{ _method = value; return *this; }

	public:
		headers_map const& headers() const ETC_NOEXCEPT
		{ return _headers; }

		headers_map& headers() ETC_NOEXCEPT
		{ return _headers; }

		Request& header(std::string const& key, std::string value)
		{ _headers[key] = std::move(value); return *this; }

	public:
		std::string const& body() const ETC_NOEXCEPT
		{ return _body; }

		Request& body(std::string value)
		{ _body = std::move(value);  return *this; }
	};

}}

#endif
