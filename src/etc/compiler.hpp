#ifndef  ETC_COMPILER_HPP
# define ETC_COMPILER_HPP

# include <boost/config.hpp>

# ifdef BOOST_NO_CXX11_DELETED_FUNCTIONS
#  define ETC_DELETED_FUNCTION
# else
#  define ETC_DELETED_FUNCTION = delete
# endif

# define ETC_NOEXCEPT BOOST_NOEXCEPT

# ifdef BOOST_MSVC
#  define ETC_PRETTY_FUNCTION __FUNCTION__
# else
#  define ETC_PRETTY_FUNCTION __PRETTY_FUNCTION__
# endif

#endif
