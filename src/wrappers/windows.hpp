#ifndef  LIB_WRAPPERS_WINDOWS_HPP
# define LIB_WRAPPERS_WINDOWS_HPP

# ifdef _WIN32
#  include <Windows.h>
#  include <Winbase.h>
#  undef DELETE
# endif

#endif
