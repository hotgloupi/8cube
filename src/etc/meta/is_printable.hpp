#ifndef  ETC_META_IS_PRINTABLE_HPP
# define ETC_META_IS_PRINTABLE_HPP

# include "clean_type.hpp"

# include <iosfwd>

namespace etc { namespace meta {

		template<typename T>
		struct is_printable
		{
			typedef typename meta::clean_type<T>::type clean_type;
			struct yes { static bool const value = true; };
			struct no { static bool const value = false; };

			template<int i> struct helper { typedef int type; };
			static no sfinae(...);
			template<typename U>
			static yes sfinae(
				U*,
				typename helper<
					sizeof(
						std::declval<std::ostream&>() << std::declval<U const&>()
					)
				>::type val = 0);

			static bool const value = decltype(
				sfinae(std::declval<clean_type*>())
			)::value;
		};

}}

#endif
