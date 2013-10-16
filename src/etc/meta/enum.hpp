#ifndef  ETC_META_ENUM_HPP
# define ETC_META_ENUM_HPP

# include <etc/types.hpp>

/// Best signed integer type for the enum.
# define ETC_META_ENUM_STORAGE(__type)                                        \
	typename ::etc::meta::enum_detail::StorageType<sizeof(__type)>::type      \
/**/

/// Convert enum value to a signed integer value.
# define ETC_META_ENUM_INDEXOF(__type, __value)                               \
	static_cast<ETC_META_ENUM_STORAGE(__type)>(__type::__value)               \
/**/

namespace etc { namespace meta {

	namespace enum_detail {

		template<size_t const size>
		struct StorageType;

# define _ETC_META_ENUM_STORAGE(__type)                                       \
		template<> struct StorageType<sizeof(__type)>                         \
		{                                                                     \
			typedef __type type;                                              \
		}                                                                     \
/**/
		_ETC_META_ENUM_STORAGE(int8_t);
		_ETC_META_ENUM_STORAGE(int16_t);
		_ETC_META_ENUM_STORAGE(int32_t);
		_ETC_META_ENUM_STORAGE(int64_t);

# undef _ETC_META_ENUM_STORAGE

	}

	template<typename E>
	struct enum_
	{
		typedef
			typename enum_detail::StorageType<sizeof(E)>::type
			storage_type;

		static storage_type const max = ETC_META_ENUM_INDEXOF(E, _maxvalue);

		static storage_type indexof(E value)
		{
			return static_cast<storage_type>(value);
		}

		static bool valid_index(E value, storage_type const start = 0)
		{
			return (
				static_cast<storage_type>(value) >= start &&
				static_cast<storage_type>(value) < max
			);
		}
	};

}}

#endif
