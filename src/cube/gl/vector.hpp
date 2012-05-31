#ifndef  CUBE_GL_VECTOR_HPP
# define CUBE_GL_VECTOR_HPP

# include <glm/glm.hpp>
# include <glm/core/type_vec2.hpp>
# include <glm/core/type_vec3.hpp>
//# include <glm/gtx/transform.hpp>

namespace cube { namespace gl { namespace vector {

	using namespace ::glm;

	template<typename T>
		using Vector2 = ::glm::detail::tvec2<T>;
	template<typename T>
		using Vector3 = ::glm::detail::tvec3<T>;

	typedef Vector2<float> Vector2f;
	typedef Vector3<float> Vector3f;

}}}

#endif /* ! VECTOR_HPP */
