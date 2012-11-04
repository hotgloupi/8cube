#ifndef  CUBE_GL_VECTOR_HPP
# define CUBE_GL_VECTOR_HPP

# include <glm/glm.hpp>
# include <glm/core/type_vec2.hpp>
# include <glm/core/type_vec3.hpp>
//# include <glm/gtx/transform.hpp>

# include "fwd.hpp"
# include "content_traits.hpp"

# include <iosfwd>

namespace cube { namespace gl { namespace vector {

	using namespace ::glm;

	template<typename T> using Vector3 = detail::tvec3<T>;
	template<typename T> using Vector2 = detail::tvec2<T>;

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
	_CUBE_GL_VECTOR_DEF(3, float, f);

# undef _CUBE_GL_VECTOR_DEF


#endif /* ! VECTOR_HPP */
