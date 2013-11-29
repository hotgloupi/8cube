#ifndef  CUBE_GL_MATRIX_HPP
# define CUBE_GL_MATRIX_HPP

# include "fwd.hpp"
# include "vector.hpp"

# include <cube/api.hpp>
# include <cube/units/angle.hpp>

# include <etc/types.hpp>

# include <glm/glm.hpp>
# include <glm/core/type_mat4x4.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtx/transform.hpp>

# include <iosfwd>

namespace cube { namespace gl { namespace matrix {

	template<typename T>
	inline
	typename std::enable_if<
		std::is_floating_point<T>::value, Matrix44<T>
	>::type
	perspective(T const fov, T const aspect, T const near, T const far)
	{
		return ::glm::perspective(fov, aspect, near, far);
	}

	template<typename T>
	inline
	typename std::enable_if<
		std::is_floating_point<T>::value, Matrix44<T>
	>::type
	look_at(vector::Vector3<T> const& eye,
	        vector::Vector3<T> const& center,
	        vector::Vector3<T> const& up)
	{
		return ::glm::lookAt(eye, center, up);
	}

	template<typename T>
	inline
	typename std::enable_if<std::is_floating_point<T>::value, Matrix44<T>>::type
	translate(Matrix44<T> const& m, vector::Vector3<T> const& v)
	{
		return ::glm::translate(m, v);
	}

	template<typename T>
	inline
	typename std::enable_if<std::is_floating_point<T>::value, Matrix44<T>>::type
	translate(Matrix44<T> const& m, T const x, T const y, T const z)
	{
		return ::glm::translate(m, x, y, z);
	}

	template<typename T>
	inline
	typename std::enable_if<std::is_floating_point<T>::value, Matrix44<T>>::type
	scale(Matrix44<T> const& m, T const x, T const y, T const z)
	{
		return ::glm::scale(m, x, y, z);
	}

	template<typename T>
	inline
	typename std::enable_if<std::is_floating_point<T>::value, Matrix44<T>>::type
	rotate(Matrix44<T> const& m,
	       units::Angle const angle,
	       T const x, T const y, T const z)
	{
		return ::glm::rotate(m, units::deg_value(angle), x, y, z);
	}

	template<typename T>
	inline
	typename std::enable_if<std::is_floating_point<T>::value, Matrix44<T>>::type
	rotate(Matrix44<T> const& m,
	       units::Angle const angle,
	       vector::Vector3<T> const& axis)
	{
		return ::glm::rotate(m, units::deg_value(angle), axis);
	}

	template<typename T>
	inline
	typename std::enable_if<std::is_floating_point<T>::value, Matrix44<T>>::type
	ortho(T const x, T const y, T const w, T const h)
	{
		return ::glm::ortho(x, y, w, h);
	}

}}}

CUBE_API
std::ostream&
operator <<(std::ostream& out,
            cube::gl::matrix::Matrix44f const& m);

#endif /* ! MATRIX_HPP */



