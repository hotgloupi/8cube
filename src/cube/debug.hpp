#ifndef  CUBE_DEBUG_HPP
# define CUBE_DEBUG_HPP

# include "debug/Performance.hpp"

# include <etc/compiler.hpp>

# include <boost/preprocessor/cat.hpp>

	/**
	 * @brief Compute a scope block performance statistics.
	 *
	 * Doing so in important parts of the code source give the ability to
	 * measure hierarchically impact of the code.
	 *
	 * @see @a Performace class to retreive statistics.
	 */
# define CUBE_DEBUG_PERFORMANCE_SECTION(name)                                 \
	::cube::debug::Section< ::cube::debug::Performance>                       \
		BOOST_PP_CAT(__cube_debug_performance_section_, __LINE__){            \
			name,                                                             \
			__FILE__,                                                         \
			__LINE__,                                                         \
			ETC_PRETTY_FUNCTION                                               \
		}                                                                     \
/**/

#endif
