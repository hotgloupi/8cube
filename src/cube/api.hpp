#ifndef  CUBE_API_HPP
# define CUBE_API_HPP

# include <etc/api.hpp>

# ifdef CUBE_BUILD_DYNAMIC_LIBRARY
#  define CUBE_API             ETC_API_EXPORT
# else
#  define CUBE_API             ETC_API_IMPORT
# endif

# define CUBE_API_EXCEPTION   ETC_API_EXCEPTION

#endif
