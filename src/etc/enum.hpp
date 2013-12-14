#ifndef ETC_ENUM_HPP
# define ETC_ENUM_HPP

# include "types.hpp"

namespace etc {

	// Cast any enums to integer.
	template <typename To = size_t, typename E>
	inline ETC_CONSTEXPR
	typename std::enable_if<std::is_enum<E>::value, To>::type
	enum_cast(E const value)
	{
		static_assert(sizeof(E) <= sizeof(To),
		              "Destination type is too short.");
		return static_cast<To>(value);
	}

	// Hash enums in an unordered container easily.
	struct enum_hash
	{
		template <typename E>
		inline ETC_CONSTEXPR
		size_t operator ()(E const value) const
		{
			return ::etc::enum_cast<size_t>(value);
		}
	};

}

#endif
