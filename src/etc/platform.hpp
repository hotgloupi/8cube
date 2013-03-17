#ifndef  ETC_PLATFORM_HPP
# define ETC_PLATFORM_HPP

# ifdef _WIN32
#  define ETC_PLATFORM_WINDOWS 1
# elif defined(__APPLE__) or defined(__apple__) or defined(__darwin__)
#  define ETC_PLATFORM_MACOSX 1
#  define ETC_PLATFORM_DARWIN 1
# elif defined(__linux__)
#  define ETC_PLATFORM_LINUX 1
# else
#  error "Unknown platform !"
# endif

#endif

