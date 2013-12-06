#ifndef  ETC_COMPILER_HPP
# define ETC_COMPILER_HPP

# include <boost/config.hpp>
# include <boost/preprocessor/config/config.hpp>

# ifdef BOOST_NO_CXX11_DELETED_FUNCTIONS
#  define ETC_DELETED_FUNCTION
# else
#  define ETC_DELETED_FUNCTION = delete
# endif

# define ETC_NOEXCEPT BOOST_NOEXCEPT

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
