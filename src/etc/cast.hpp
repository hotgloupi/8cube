#ifndef  ETC_CAST_HPP
# define ETC_CAST_HPP

# include <etc/assert.hpp>

# include <type_traits>

namespace etc {

	namespace detail {

		template<typename T, typename Ret = void>
		struct enable_polymorphic
			: std::enable_if<
				std::has_virtual_destructor<
					typename std::remove_pointer<
						typename std::remove_reference<T>::type
					>::type
				>::value
				, Ret
			>
		{};

		template<typename T, typename Ret = void>
		struct disable_polymorphic
			: std::enable_if<
				!std::has_virtual_destructor<
					typename std::remove_pointer<
						typename std::remove_reference<T>::type
					>::type
				>::value
				, Ret
			>
		{};

	}

	// Non-polymorphic cast.
	template<typename To, typename From>
	auto cast(From&& from)
		-> typename detail::disable_polymorphic<From, To>::type
	{ return static_cast<To>(std::forward<From>(from)); }

	// Polymorphic pointer cast.
	template<typename To, typename From>
	auto cast(From* from)
		-> typename detail::enable_polymorphic<From, To>::type
	{
		ETC_ASSERT_EQ(dynamic_cast<To>(from), static_cast<To>(from));
		return static_cast<To>(from);
	}

	// Polymorphic reference cast.
	template<typename To, typename From>
	auto cast(From& from)
		-> typename detail::enable_polymorphic<From, To>::type
	{
		typedef typename std::remove_reference<To>::type* to_ptr_type;
		ETC_ASSERT_EQ(
			dynamic_cast<to_ptr_type>(&from),
			static_cast<to_ptr_type>(&from)
		);
		return static_cast<To>(from);
    }

	// Unique pointer cast.
	template<typename To, typename From>
	std::unique_ptr<To> cast(std::unique_ptr<From> from)
	{
		std::unique_ptr<To> res{cast<To*>(from.get())};
		from.release();
		return std::move(res);
	}

	// Shared pointer cast.
	template<typename To, typename From>
	std::shared_ptr<To> cast(std::shared_ptr<From> const& from)
	{
		typedef typename std::remove_reference<To>::type* to_ptr_type;
		ETC_ASSERT_EQ(
			dynamic_cast<to_ptr_type>(from.get()),
			static_cast<to_ptr_type>(from.get())
		);
		return std::static_pointer_cast<To>(from);
	}

}

#endif
