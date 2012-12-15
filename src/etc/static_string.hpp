#ifndef  ETC_STATIC_STRING_HPP
# define ETC_STATIC_STRING_HPP

# include <iosfwd>
# include <type_traits>

namespace etc {

	namespace detail {

		template<typename T>
		struct IsArray
		{ static bool const value = false; };

		template<typename T>
		struct IsArray<T&>
		{ static bool const value = IsArray<T>::value; };

		template<unsigned int size, typename CharType>
		struct IsArray<CharType const[size]>
		{ static bool const value = true; };

	}

	template <unsigned int size_, typename CharType = char>
	struct StaticString
	{
	public:
		static unsigned int const size = size_;

	public:
		CharType const data[size_];

	public:
		inline constexpr
		StaticString(StaticString const& other) = default;

		template<typename T>
		inline constexpr
		StaticString(T&& other,
		             typename std::enable_if<
		                 detail::IsArray<T>::value
		               , bool
		             >::type = true)
			: StaticString{*((StaticString<sizeof(T)> const*) other)}
		{}

	};

	template<typename T>
	inline constexpr
	typename std::enable_if<
		detail::IsArray<T>::value
	  , StaticString<sizeof(T) - 1>
	>::type
	static_string(T&& value)
	{
		return *((StaticString<sizeof(T) - 1>*) value);
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
