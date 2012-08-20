#include "Exception.hpp"
#include "_opengl.hpp"

#include <map>

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
		GL_VERTEX_ARRAY, 0, GL_COLOR_ARRAY, GL_NORMAL_ARRAY,
		GL_TEXTURE0_ARB, GL_TEXTURE1_ARB, GL_TEXTURE2_ARB,
	};

	GLenum gl::_content_hint_map[(size_t)ContentHint::_max_value] = {
		GL_STREAM_DRAW, GL_STATIC_DRAW, GL_DYNAMIC_DRAW,
	};

	GLenum gl::_shader_type_map[(size_t)ShaderType::_max_value] = {
		GL_FRAGMENT_SHADER,
		GL_VERTEX_SHADER,
	};

	void gl::_check_error(char const* function_)
	{
		GLenum code = glGetError();

		if (code == GL_NO_ERROR)
			return;

		static std::map<GLenum, std::pair<std::string, std::string>> errors{
#define _ERR(enum, str)                                                       \
	{enum, {#enum, str "."}}                                                  \
	/**/
			_ERR(GL_INVALID_ENUM, "Wrong enum value"),
			_ERR(GL_INVALID_VALUE, "Wrong value"),
			_ERR(GL_INVALID_OPERATION, "Wrong operation"),
			_ERR(GL_STACK_OVERFLOW, "Stack overflow"),
			_ERR(GL_OUT_OF_MEMORY, "Out of memory"),
			_ERR(GL_INVALID_FRAMEBUFFER_OPERATION_EXT,
				 "Wrong operation on a frame buffer"),
#undef _ERR
		};

		std::string error, description;
		auto it = errors.find(code);
		if (it == errors.end())
		{
			error = etc::to_string("Unknown error code", code);
			description = "Not documented error.";
		}
		else
		{
			error = it->second.first;
			description = it->second.second;
		}

		std::string function = (
			function_ != nullptr ? function_ : "(anonymous function)"
		);
        throw Exception{
			std::string{function} + ": " + error + ": " + description
		};
    }

}}} // !cube::gl::opengl
