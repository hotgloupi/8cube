#ifndef  CUBE_GL_CONTENT_TRAITS_HPP
# define CUBE_GL_CONTENT_TRAITS_HPP

# include <etc/types.hpp>

namespace cube { namespace gl {

	/**
	 * @brief Defines traits for any basic types.
	 *
	 *  component_type: type for internal component value.
	 *  arity: The number of components.
	 *
	 *  @note Every types T that have an associated content_traits TR verify
	 *        sizeof(T) == sizeof(TR::component_type) * TR::arity
	 */
	template<typename T>
	struct content_traits;

	template<typename T>
	struct content_traits<T const> : content_traits<T> {};

# define _CUBE_GL_CONTENT_TRAITS(__type)                                      \
	template<>                                                                \
	struct content_traits<__type>                                             \
	{                                                                         \
		static unsigned int const arity = 1;                                  \
		typedef __type component_type;                                        \
	}                                                                         \
/**/
		_CUBE_GL_CONTENT_TRAITS(uint8_t);
		_CUBE_GL_CONTENT_TRAITS(uint16_t);
		_CUBE_GL_CONTENT_TRAITS(uint32_t);

# undef _CUBE_GL_CONTENT_TRAITS

}}

#endif

