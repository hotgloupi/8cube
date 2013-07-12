#ifndef ETC_ENUM_HPP
# define ETC_ENUM_HPP

# include "types.hpp"

namespace etc {

	template <typename To = size_t, typename E>
	inline constexpr
	typename std::enable_if<std::is_enum<E>::value, To>::type
	enum_cast(E const value)
	{
		return static_cast<To>(value);
	}

	struct enum_hash
	{
		template <typename E>
		inline constexpr
		size_t operator ()(E const value) const
		{
			return ::etc::enum_cast<size_t>(value);
		}
	};

}

#endif
