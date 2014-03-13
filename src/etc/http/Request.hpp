#ifndef  ETC_HTTP_REQUEST_HPP
# define ETC_HTTP_REQUEST_HPP

# include "fwd.hpp"

# include <etc/compiler.hpp>

# include <map>
# include <memory>

namespace etc { namespace http {

	/**
	 * Named arguments representing a request.
	 *
	 * This class is meant for two purposes:
	 *  - Construct a request when needed.
	 *  - Store a request template and make a copy when needed.
	 *
	 * For example:
	 * ------------------------------------------------------------------------
	 * Client c("http://example.com");
	 * c.fire(
	 *    Request(Method::post)
	 *       .url("/login")
	 *       .header("Content-Type", "application/json")
	 *       .body("{\"login\": \"test\",\"password\": \"test\"}")
	 * )
	 * ------------------------------------------------------------------------
	 */
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
		explicit Request(Method method = Method::get);
		Request(Request const& other);
		Request(Request&& other);
		~Request();

	public:
		/// URL (Always starts with "/")
		std::string const& url() const ETC_NOEXCEPT
		{ return _url; }

		/// Set the URL.
		/// A '/' will always be prepended to the url.
		Request& url(std::string value);

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
