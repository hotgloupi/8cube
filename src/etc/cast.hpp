#ifndef  ETC_CAST_HPP
# define ETC_CAST_HPP

# include <etc/assert.hpp>

# include <type_traits>

namespace etc {

	template<typename To, typename From>
	To cast(From&& from)
	{ return static_cast<To>(std::forward<From>(from)); }

	template<typename To, typename From>
	typename std::enable_if<std::has_virtual_destructor<From>::value, To>::type
	cast(From* from)
	{
		ETC_ASSERT_EQ(dynamic_cast<To>(from), static_cast<To>(from));
		return static_cast<To>(from);
	}

	template<typename To, typename From>
	typename std::enable_if<std::has_virtual_destructor<From>::value, To>::type
	cast(From& from)
	{
		typedef typename std::remove_reference<To>::type* to_ptr_type;
		ETC_ASSERT_EQ(
			dynamic_cast<to_ptr_type>(&from),
			static_cast<to_ptr_type>(&from)
		);
		return static_cast<To>(from);
    }

	template<typename To, typename From>
	std::unique_ptr<To> cast(std::unique_ptr<From> from)
	{
		std::unique_ptr<To> res{cast<To*>(from.get())};
		from.release();
		return std::move(res);
	}

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
