#ifndef  LIB_WRAPPERS_WINDOWS_HPP
# define LIB_WRAPPERS_WINDOWS_HPP

# ifdef _WIN32
#  ifndef NOMINMAX
#   define NOMINMAX
#  endif
#  ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#  endif
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
