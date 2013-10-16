#ifndef  ETC_STATIC_STRING_HPP
# define ETC_STATIC_STRING_HPP

# include <iosfwd>
# include <type_traits>

namespace etc {

	namespace detail {

		template<typename T>
		struct IsArray
			: std::is_array<
			    typename std::remove_cv<
			      typename std::remove_reference<T>::type
			    >::type
			  >
		{};

		template<typename T>
		struct Extent
			: std::extent<
			    typename std::remove_cv<
			      typename std::remove_reference<T>::type
			    >::type
			  >
		{};
	}

	template<unsigned int size_,
	         typename CharType = char>
	struct StaticString
	{
	public:
		static unsigned int const size = size_;

	public:
		CharType const data[size_];

	public:
		inline constexpr
		StaticString(StaticString const&) = default;

		inline
		StaticString& operator =(StaticString const&) = delete;

		inline constexpr
		StaticString(StaticString&&) = default;

		inline
		StaticString& operator =(StaticString&&) = delete;

		template<typename T,
		         typename std::enable_if<detail::IsArray<T>::value, int>::type = 0>
		inline constexpr
		StaticString(T&& other)
			: StaticString{*((StaticString const*) other)}
		{}
	};

	template<typename T>
	inline constexpr
	typename std::enable_if<
		  detail::IsArray<T>::value
		, StaticString<detail::Extent<T>::value>
	>::type
	static_string(T&& value)
	{
		return *((StaticString<detail::Extent<T>::value>*) value);
	}

	namespace detail {

		// Helper to avoid including iostream
		std::ostream& print_string(std::ostream& out,
		                           char const* data,
		                           unsigned int size);

	}

	template <unsigned int size, typename CharType = char>
	inline
	std::ostream& operator <<(std::ostream& out,
	                          StaticString<size, CharType> const& val)
	{
		return detail::print_string(out, val.data, val.size);
	}

}

#endif
