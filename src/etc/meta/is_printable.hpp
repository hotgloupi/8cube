#ifndef  ETC_META_IS_PRINTABLE_HPP
# define ETC_META_IS_PRINTABLE_HPP

# include "clean_type.hpp"

# include <iosfwd>

namespace etc { namespace meta {

		namespace detail {

			template<typename T>
			struct is_printable
			{
				typedef char yes[1];
				typedef char no[2];

				static std::ostream& stream();
				typedef typename meta::clean_type<T>::type type;

				template<typename U>
				static
				typename std::enable_if<
					true
#ifdef BOOST_MSVC
					// enum class object cause an internal compiler error ...
					&& !std::is_enum<U>::value
#endif
					, yes&
				>::type
				test(
					decltype(*static_cast<std::ostream*>(0) << * static_cast<U*>(0)));

				template<typename U> static no& test(...);

				static bool const value = sizeof(test<type>(stream())) == sizeof(yes);
			};

		}

		template<typename T>
		struct is_printable
			: public detail::is_printable<T>
		{};

}}

#endif
