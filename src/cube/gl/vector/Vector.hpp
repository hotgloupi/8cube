#ifndef  CUBE_GL_VECTOR_VECTOR_HPP
# define CUBE_GL_VECTOR_VECTOR_HPP

# include <cube/gl/fwd.hpp>

# include <cube/api.hpp>
# include <cube/gl/content_traits.hpp>
# include <cube/units/angle.hpp>

# include <glm/glm.hpp>
# include <glm/geometric.hpp>
# include <glm/vec2.hpp>
# include <glm/vec3.hpp>
# include <glm/vec4.hpp>
# include <glm/gtx/rotate_vector.hpp>

# include <boost/functional/hash.hpp>

# include <iosfwd>

namespace cube { namespace gl { namespace vector {

	template<typename T>
	inline
	Vector3<T> rotate_x(Vector3<T> const& v, units::Angle const& angle)
	{
		return glm::rotateX(v, units::rad_value<T>(angle));
	}

	template<typename T>
	inline
	Vector3<T> rotate_y(Vector3<T> const& v, units::Angle const& angle)
	{
		return glm::rotateY(v, units::rad_value<T>(angle));
	}

	template<typename T>
	inline
	Vector3<T> rotate_z(Vector3<T> const& v, units::Angle const& angle)
	{
		return glm::rotateZ(v, units::rad_value<T>(angle));
	}

	template<typename T>
	inline
	Vector3<T> rotate(Vector3<T> const& v,
	                  units::Angle const& angle,
	                  Vector3<T> const& normal)
	{
		return glm::rotate(v, units::rad_value<T>(angle), normal);
	}

	using glm::cross;
	using glm::dot;
	using glm::distance;
	using glm::normalize;

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

	template<typename T>
	struct content_traits<vector::Vector4<T>>
	{
		static unsigned int const arity = 4;
		typedef T component_type;
	};

}}

# define _CUBE_GL_VECTOR_DEF(__arity, __type, __suffix)                       \
namespace glm { namespace detail {                                            \
	CUBE_API                                                                  \
	std::ostream& operator <<(                                                \
	    std::ostream& out,                                                    \
	    cube::gl::vector::Vector ## __arity<__type> const& value);            \
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

	_CUBE_GL_VECTOR_DEF(4, float, f);
	_CUBE_GL_VECTOR_DEF(4, double, d);
	_CUBE_GL_VECTOR_DEF(4, int32_t, i);
	_CUBE_GL_VECTOR_DEF(4, uint32_t, u);
	_CUBE_GL_VECTOR_DEF(4, int64_t, il);
	_CUBE_GL_VECTOR_DEF(4, uint64_t, ul);
# undef _CUBE_GL_VECTOR_DEF

namespace std {

	template<typename T> struct hash<cube::gl::vector::Vector2<T>>
	{
		size_t operator ()(cube::gl::vector::Vector2<T> const& v) const
		{
			size_t seed = boost::hash_value(v.x);
			boost::hash_combine(seed, v.y);
			return seed;
		}
	};

	template<typename T> struct hash<cube::gl::vector::Vector3<T>>
	{
		size_t operator ()(cube::gl::vector::Vector3<T> const& v) const
		{
			size_t seed = boost::hash_value(v.x);
			boost::hash_combine(seed, v.y);
			boost::hash_combine(seed, v.z);
			return seed;
		}
	};

	template<typename T> struct hash<cube::gl::vector::Vector4<T>>
	{
		size_t operator ()(cube::gl::vector::Vector4<T> const& v) const
		{
			size_t seed = boost::hash_value(v.x);
			boost::hash_combine(seed, v.y);
			boost::hash_combine(seed, v.z);
			boost::hash_combine(seed, v.w);
			return seed;
		}
	};

}

#endif /* ! VECTOR_HPP */
