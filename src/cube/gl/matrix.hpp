#ifndef  CUBE_GL_MATRIX_HPP
# define CUBE_GL_MATRIX_HPP

# include <glm/glm.hpp>
# include <glm/core/type_mat4x4.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtx/transform.hpp>

# include <iosfwd>

namespace cube { namespace gl { namespace matrix {

	using namespace ::glm;

	typedef ::glm::detail::tmat4x4<float> mat44f;


}}}

std::ostream&
operator <<(std::ostream& out,
            cube::gl::matrix::mat44f const& m);

#endif /* ! MATRIX_HPP */



