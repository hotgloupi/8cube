#ifndef  CUBE_GL_VECTOR_HPP
# define CUBE_GL_VECTOR_HPP

# include <glm/glm.hpp>
# include <glm/core/type_vec2.hpp>
# include <glm/core/type_vec3.hpp>
//# include <glm/gtx/transform.hpp>

# include "fwd.hpp"

# include <iosfwd>

namespace cube { namespace gl { namespace vector {

	using namespace ::glm;

# define _CUBE_GL_VECTOR_DEF(arity_)                                          \
	template<typename T>                                                      \
	struct Vector##arity_                                                     \
		: public detail::tvec##arity_<T>                                      \
	{                                                                         \
		static const unsigned int arity = arity_;                             \
		typedef T component_t;                                                \
		template<typename... K>                                               \
		Vector##arity_(K... values)                                           \
			: detail::tvec##arity_<T>{values...}                              \
		{};                                                                   \
		explicit                                                              \
		Vector##arity_(detail::tvec##arity_<T> const& val)                    \
			: detail::tvec##arity_<T>{val}                                    \
		{};                                                                   \
	};                                                                        \
	typedef Vector##arity_<float> Vector##arity_##f;                          \
	std::ostream& operator <<(std::ostream& out,                              \
	                          Vector##arity_<float> const& value);            \
	/**/

	_CUBE_GL_VECTOR_DEF(2);
	_CUBE_GL_VECTOR_DEF(3);

# undef _CUBE_GL_VECTOR_DEF


}}}

#endif /* ! VECTOR_HPP */
