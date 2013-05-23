#ifndef  CUBE_MAIN_PROTO_HPP
# define CUBE_MAIN_PROTO_HPP

# include <etc/platform.hpp>

/**
 * @brief Macro to choose the right main function name.
 *
 * While on most system, the function name can be `int main()`, SDL on MacOSX
 * requires on startup initialization done in SDLMain library. In that case,
 * we have to name our main function after SDL's requirement.
 */
# if ETC_PLATFORM_MACOSX // XXX and SDLMain
#  define CUBE_MAIN_FUNC_NAME SDL_main
# else
#  define CUBE_MAIN_FUNC_NAME main
# endif

# define CUBE_MAIN_PROTO \
extern "C" int CUBE_MAIN_FUNC_NAME(int, char**); \
static int _cube_main(int, char**); \
extern "C" int CUBE_MAIN_FUNC_NAME(int ac, char** av) \
{ \
	try \
	{ \
		return _cube_main(ac, av); \
	} \
	catch (...) \
	{ \
		return 1; \
	} \
}\
static int _cube_main \
/**/

#endif

