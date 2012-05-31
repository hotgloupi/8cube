#ifndef  CUBE_GL_VECTOR_HPP
# define CUBE_GL_VECTOR_HPP

# include <glm/glm.hpp>
# include <glm/core/type_vec2.hpp>
# include <glm/core/type_vec3.hpp>
//# include <glm/gtx/transform.hpp>

namespace cube { namespace gl { namespace vector {

	using namespace ::glm;

# define _CUBE_GL_VECTOR_DEF(arity)                                           \
	template<typename T>                                                      \
	struct Vector##arity                                                      \
		: public detail::tvec##arity<T>                                       \
	{                                                                         \
		template<typename... K>                                               \
		Vector##arity(K... values)                                            \
			: detail::tvec##arity<T>{values...}                               \
		{};                                                                   \
	};                                                                        \
	typedef Vector##arity<float> Vector##arity##f                             \
	/**/

	_CUBE_GL_VECTOR_DEF(2);
	_CUBE_GL_VECTOR_DEF(3);

# undef _CUBE_GL_VECTOR_DEF


}}}

#endif /* ! VECTOR_HPP */
