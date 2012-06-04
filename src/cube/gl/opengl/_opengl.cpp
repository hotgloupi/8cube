#include "_opengl.hpp"

namespace cube { namespace gl { namespace opengl {


	GLenum gl::_draw_mode_map[(size_t)DrawMode::_max_value] = {
		GL_POINTS,
		GL_LINES,
		GL_LINE_STRIP,
		GL_LINE_LOOP,
		GL_TRIANGLES,
		GL_TRIANGLE_STRIP,
		GL_TRIANGLE_FAN,
		GL_QUADS,
		GL_QUAD_STRIP,
		GL_POLYGON,
	};

	GLenum gl::_content_type_map[(size_t)ContentType::_max_value] = {
		GL_BYTE,    GL_UNSIGNED_BYTE,   GL_SHORT,   GL_UNSIGNED_SHORT,
		GL_INT,     GL_UNSIGNED_INT,    GL_FLOAT
	};

	GLenum gl::_content_kind_map[(size_t)ContentKind::_max_value]  = {
		GL_VERTEX_ARRAY, GL_INDEX_ARRAY, GL_COLOR_ARRAY, GL_NORMAL_ARRAY,
		GL_TEXTURE0_ARB, GL_TEXTURE1_ARB, GL_TEXTURE2_ARB,
	};

	GLenum gl::_content_hint_map[(size_t)ContentHint::_max_value] = {
		GL_STREAM_DRAW, GL_STATIC_DRAW, GL_DYNAMIC_DRAW,
	};

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
