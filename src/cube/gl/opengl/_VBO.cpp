#include "_VBO.hpp"

namespace cube { namespace gl { namespace opengl {

	gl::SubVBO::pointer_method_t
		gl::SubVBO::_pointer_methods[(size_t)ContentKind::_max_value] = {
			&gl::SubVBO::vertex_pointer,
			&gl::SubVBO::index_pointer,
			&gl::SubVBO::color_pointer,
			&gl::SubVBO::normal_pointer,
			&gl::SubVBO::tex_coord0_pointer,
			&gl::SubVBO::tex_coord1_pointer,
			&gl::SubVBO::tex_coord2_pointer,
	};

	template<>
	const GLint  gl::VBO<true>::_gl_array_type = GL_ELEMENT_ARRAY_BUFFER_ARB;
	template<>
	const GLint  gl::VBO<false>::_gl_array_type = GL_ARRAY_BUFFER_ARB;

}}} // !cube::gl::opengl
