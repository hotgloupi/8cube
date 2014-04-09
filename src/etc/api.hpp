#ifndef  ETC_API_HPP
# define ETC_API_HPP

/**
 * @brief This modules declares macros for importing/exporting symbols inside
 *        libetc.
 *
 * Macros defined here:
 *  - `ETC_API': Declares a symbol as part of the API
 *  - `ETC_API_EXCEPTION': Same as before, but for exceptions.
 *  - `ETC_API_INTERNAL': This symbol should be hidden.
 *
 * @warning When building etc as a dynamic library, you must ensure that you
 *          define `ETC_BUILD_DYNAMIC_LIBRARY'.
 */

# include "platform.hpp"

# include <boost/config.hpp>

# ifdef ETC_PLATFORM_WINDOWS
#  define ETC_API_IMPORT    __declspec(dllimport)
#  define ETC_API_EXPORT    __declspec(dllexport)
#  define ETC_API_INTERNAL  /* internal */
# else
#  define ETC_API_IMPORT    /* import */
#  define ETC_API_EXPORT    __attribute__((visibility("default")))
#  define ETC_API_INTERNAL  __attribute__((visibility("hidden")))
# endif


# ifdef ETC_BUILD_DYNAMIC_LIBRARY
#  define ETC_API             ETC_API_EXPORT
# elif defined(ETC_BUILD_STATIC_LIBRARY) || defined(ETC_STATIC_LIBRARY)
#  define ETC_API             /* nothing */
# else
#  define ETC_API             ETC_API_IMPORT
# endif

# ifdef ETC_PLATFORM_WINDOWS
#  define ETC_API_EXCEPTION   ETC_API
# else
#  define ETC_API_EXCEPTION   ETC_API_EXPORT // on gcc export everywhere
# endif

# ifdef BOOST_MSVC
// non exported type in exported ones need to have a dll-interface.
// This is ignored since boost is not binary compatible, so are we.
#  pragma warning(disable: 4251) // for members
#  pragma warning(disable: 4275) // for base class

# endif

#endif
