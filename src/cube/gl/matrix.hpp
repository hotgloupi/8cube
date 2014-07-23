#ifndef  CUBE_GL_MATRIX_HPP
# define CUBE_GL_MATRIX_HPP

# include "fwd.hpp"
# include "vector.hpp"

# include <cube/api.hpp>
# include <cube/units/angle.hpp>

# include <etc/compiler.hpp>
# include <etc/types.hpp>

# include <glm/glm.hpp>
# include <glm/mat4x4.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtx/transform.hpp>

# include <iosfwd>

namespace cube { namespace gl { namespace matrix {

	using glm::perspective;
	using glm::translate;
	using glm::scale;
	using glm::ortho;

	template<typename T>
	inline
	Matrix44<T> look_at(vector::Vector3<T> const& eye,
	                    vector::Vector3<T> const& center,
	                    vector::Vector3<T> const& up) ETC_NOEXCEPT
	{
		return ::glm::lookAt(eye, center, up);
	}

	template<typename T>
	inline
	Matrix44<T> rotate(Matrix44<T> const& mat,
	                   units::Angle const angle,
	                   vector::Vector3<T> const& axis) ETC_NOEXCEPT
	{ return ::glm::rotate(mat, units::rad_value(angle), axis); }

}}}

CUBE_API
std::ostream&
operator <<(std::ostream& out,
            cube::gl::matrix::mat4f const& m);

#endif /* ! MATRIX_HPP */



