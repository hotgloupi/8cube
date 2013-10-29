#ifndef  LIB_WRAPPERS_WINDOWS_HPP
# define LIB_WRAPPERS_WINDOWS_HPP

# ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <Windows.h>
#  include <Winbase.h>
#  include <Shlobj.h>
#  include <Shlwapi.h>
#  undef DELETE
#  undef far
#  undef near
#  undef environ
# endif

#endif
