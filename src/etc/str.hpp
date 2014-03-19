#ifndef  ETC_STR_HPP
# define ETC_STR_HPP

# include <etc/compiler.hpp>

# include <string>
# include <iosfwd>

namespace etc {

	struct str
	{
	private:
		std::string _data;

	public:
		str(std::string s);
		str();
		str(str&& other);
		str(str const& other);

	private:
		str& operator =(str const& other) ETC_DELETED_FUNCTION;
		str& operator =(str&& other) ETC_DELETED_FUNCTION;

	public:
		std::string const& data() const ETC_NOEXCEPT { return _data; }

	public:
		bool empty() const { return _data.empty(); }

	public:
		str replace(std::string const& old,
		             std::string const& new_) const;

	public:
		/// Compare with str instance.
		bool operator ==(str const& other) const
		{ return _data == other._data; }

		/// Compare with a boolean.
		bool operator ==(bool truth_value) const
		{ return static_cast<bool>(*this) == truth_value; }

		/// Compare with string-like object.
		template<typename Other>
		bool operator ==(Other&& other) const
		{ return _data == std::forward<Other>(other); }

		// Forward to ==
		template<typename Other>
		bool operator !=(Other&& other) const
		{ return !(*this == std::forward<Other>(other)); }


		/// Not true when empty.
		bool operator !() const
		{ return _data.empty(); }

	public:
		/// Happily convert to string
		operator std::string() const { return _data; }

		/// Convert to bool (True if not empty)
#if defined(BOOST_NO_EXPLICIT_CONVERSION_OPERATORS)
		// Restrict conversion to bool only, not char or integers. This
		// prevents string::string(char) from beeing called by implicit
		// conversion.
		template<
			typename T,
			typename std::enable_if<std::is_same<T, bool>::value, int>::type = 0
		>
		operator T() const
#else
		explicit operator bool() const
#endif
		{ return !_data.empty(); }
	};

	std::ostream& operator <<(std::ostream& out, str const& s);

}

#endif
