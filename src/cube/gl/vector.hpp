#ifndef  CUBE_GL_VECTOR_HPP
# define CUBE_GL_VECTOR_HPP

# include "fwd.hpp"
# include "content_traits.hpp"

# include <glm/glm.hpp>
# include <glm/core/type_vec2.hpp>
# include <glm/core/type_vec3.hpp>
# include <glm/gtx/rotate_vector.hpp>
//# include <glm/gtx/transform.hpp>

# include <iosfwd>

namespace cube { namespace gl { namespace vector {

	template<typename T>
	inline
	Vector3<T> rotate_x(Vector3<T> const& v, T const angle)
	{
		return glm::rotateX(v, angle);
	}

	template<typename T>
	inline
	Vector3<T> rotate_y(Vector3<T> const& v, T const angle)
	{
		return glm::rotateY(v, angle);
	}

	template<typename T>
	inline
	Vector3<T> rotate_z(Vector3<T> const& v, T const angle)
	{
		return glm::rotateZ(v, angle);
	}

	template<typename T>
	inline
	Vector3<T> rotate(Vector3<T> const& v,
	                  T const angle,
	                  Vector3<T> const& normal)
	{
		return glm::rotate(v, angle, normal);
	}

	using glm::normalize;
	using glm::cross;

}}}

namespace cube { namespace gl {

	template<typename T>
	struct content_traits<vector::Vector2<T>>
	{
		static unsigned int const arity = 2;
		typedef T component_type;
	};

	template<typename T>
	struct content_traits<vector::Vector3<T>>
	{
		static unsigned int const arity = 3;
		typedef T component_type;
	};

}}

# define _CUBE_GL_VECTOR_DEF(__arity, __type, __suffix)                       \
namespace cube { namespace gl { namespace vector {                            \
	typedef Vector ## __arity<__type> Vector ## __arity ## __suffix;          \
}}}                                                                           \
namespace glm { namespace detail {                                            \
	std::ostream& operator <<(std::ostream& out,                              \
	                          tvec ## __arity<__type> const& value);          \
}}                                                                            \
/**/

	_CUBE_GL_VECTOR_DEF(2, float, f);
	_CUBE_GL_VECTOR_DEF(2, double, d);
	_CUBE_GL_VECTOR_DEF(2, int32_t, i);
	_CUBE_GL_VECTOR_DEF(2, uint32_t, u);
	_CUBE_GL_VECTOR_DEF(2, int64_t, il);
	_CUBE_GL_VECTOR_DEF(2, uint64_t, ul);

	_CUBE_GL_VECTOR_DEF(3, float, f);
	_CUBE_GL_VECTOR_DEF(3, double, d);
	_CUBE_GL_VECTOR_DEF(3, int32_t, i);
	_CUBE_GL_VECTOR_DEF(3, uint32_t, u);
	_CUBE_GL_VECTOR_DEF(3, int64_t, il);
	_CUBE_GL_VECTOR_DEF(3, uint64_t, ul);

# undef _CUBE_GL_VECTOR_DEF


#endif /* ! VECTOR_HPP */
