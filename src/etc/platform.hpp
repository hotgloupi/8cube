#ifndef  ETC_PLATFORM_HPP
# define ETC_PLATFORM_HPP

# ifdef _WIN32
#  define ETC_PLATFORM_WINDOWS 1
# elif defined(__APPLE__)
#  define ETC_PLATFORM_MACOSX 1
# elif defined(__linux__)
#  define ETC_PLATFORM_LINUX 1
# else
#  error "Unknown platform !"
# endif

#endif
