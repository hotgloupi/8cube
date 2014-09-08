#ifndef ETC_ENUM_HPP
# define ETC_ENUM_HPP

# include "types.hpp"

# include <array>

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

	// Return the number of enum values.
	template <typename E, typename To = size_t>
	inline ETC_CONSTEXPR
	typename std::enable_if<std::is_enum<E>::value, To>::type
	enum_size() { return enum_cast<size_t>(E::_max_value); }

	template<typename E>
	struct enum_traits
	{
		static size_t const size = static_cast<size_t>(E::_max_value);
	};

	template<typename E>
	auto enum_values()
	-> std::array<E, enum_traits<E>::size>
	{
		std::array<E, static_cast<size_t>(E::_max_value)> res;
		for (size_t i = 0; i < res.size(); i++)
			res[i] = static_cast<E>(i);
		return res;
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

	template<typename E, typename T>
	struct enum_map
	{
		typedef typename std::underlying_type<E>::type index_type;

		std::array<T, static_cast<index_type>(E::_max_value)> _value;

		inline T& operator [](E const e) ETC_NOEXCEPT
		{ return _value[static_cast<index_type>(e)]; }

		inline T const& operator [](E const e) const ETC_NOEXCEPT
		{ return _value[static_cast<index_type>(e)]; }

		inline void size() const ETC_NOEXCEPT
		{ return static_cast<index_type>(E::_max_value); }
	};


}

#endif
