#ifndef  ETC_COMPILER_HPP
# define ETC_COMPILER_HPP

# include <boost/config.hpp>
# include <boost/preprocessor/config/config.hpp>

# ifdef BOOST_NO_CXX11_DELETED_FUNCTIONS
#  define ETC_DELETED_FUNCTION
# else
#  define ETC_DELETED_FUNCTION = delete
# endif

# define ETC_NOEXCEPT BOOST_NOEXCEPT_OR_NOTHROW
# if (defined(__GNUC__) && __GNUC__ == 4 &&  __GNUC_MINOR__ <= 7 && __GNUC_PATCHLEVEL__ <= 3)
   // Prevent some internal compiler errors in gcc < 4.7.2
#  define ETC_NOEXCEPT_IF(...)
# else
#  define ETC_NOEXCEPT_IF(...) BOOST_NOEXCEPT_IF((__VA_ARGS__))
# endif
# define ETC_CONSTEXPR BOOST_CONSTEXPR

# ifndef BOOST_PP_VARIADICS
#  error "Variadic macros not enabled"
# endif

# ifdef BOOST_MSVC
#  define ETC_PRETTY_FUNCTION __FUNCTION__
#  define ETC_NOINLINE __declspec(noinline)
# else
#  define ETC_PRETTY_FUNCTION __PRETTY_FUNCTION__
#  define ETC_NOINLINE __attribute__((noinline))
# endif

#endif
