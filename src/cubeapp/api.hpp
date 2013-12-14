#ifndef  CUBEAPP_API_HPP
# define CUBEAPP_API_HPP

# include <etc/api.hpp>

# ifdef CUBEAPP_BUILD_DYNAMIC_LIBRARY
#  define CUBEAPP_API          ETC_API_EXPORT
# else
#  define CUBEAPP_API          ETC_API_IMPORT
# endif

# ifdef ETC_PLATFORM_WINDOWS
#  define CUBEAPP_API_EXCEPTION CUBEAPP_API
# else
#  define CUBEAPP_API_EXCEPTION ETC_API_EXPORT
# endif

# define CUBEAPP_API_INTERNAL   ETC_API_INTERNAL

#endif
