#ifndef  CUBE_GL_OPENGL__OPENGL_HPP
# define CUBE_GL_OPENGL__OPENGL_HPP

# include "Exception.hpp"

# include <wrappers/sdl.hpp>
# include <wrappers/opengl.hpp>

# include <etc/log.hpp>

# include <cube/gl/renderer.hpp>

# include <stdexcept>

# define _CUBE_GL_ENUM_HASHABLE(E)                                            \
	namespace std {                                                           \
		template<> struct hash<E> {                                           \
			size_t operator ()(E e) const { return static_cast<size_t>(e); }  \
		};                                                                    \
	}                                                                         \

	_CUBE_GL_ENUM_HASHABLE(cube::gl::renderer::ContentType);
	_CUBE_GL_ENUM_HASHABLE(cube::gl::renderer::ContentHint);

# undef _CUBE_GL_ENUM_HASHABLE

namespace cube { namespace gl { namespace opengl {

	using namespace ::cube::gl::renderer;

	struct gl
	{
	private:
		static void _check_error(char const* function_);

	public:
# define _CUBE_GL_OPENGL_PROTO(name, type)                                    \
		template<typename... T>                                               \
		static inline                                                         \
		type name(T... values)                                                \
	/**/

// values... is defined in the proto
# define _CUBE_GL_OPENGL_LOG(gl_name)                                         \
		ETC_LOG_COMPONENT("cube.gl.opengl._opengl");                          \
		ETC_LOG.debug(#gl_name, '(', values..., ')');                         \
	/**/

# define _CUBE_GL_OPENGL_CALL(name, gl_name)                                  \
		_CUBE_GL_OPENGL_PROTO(name, void)                                     \
		{                                                                     \
			_CUBE_GL_OPENGL_LOG(gl_name)                                      \
			::gl_name(values...);                                             \
			_check_error(#gl_name);                                           \
		}
	/**/

# define _CUBE_GL_OPENGL_CALL_RET(name, gl_name, type)                        \
		_CUBE_GL_OPENGL_PROTO(name, type)                                     \
		{                                                                     \
			_CUBE_GL_OPENGL_LOG(gl_name)                                      \
			type ret = ::gl_name(values...);                                  \
			_check_error(#gl_name);                                           \
			return ret;                                                       \
		}
	/**/

# define _CUBE_GL_OPENGL_WRAP(name)                                           \
		_CUBE_GL_OPENGL_CALL(name, gl ## name)                                \
	/**/
# define _CUBE_GL_OPENGL_WRAP_RET(name, type)                                 \
		_CUBE_GL_OPENGL_CALL_RET(name, gl ## name, type)                      \
	/**/

# define _CUBE_GL_OPENGL_WRAP_ARB(name)                                       \
		_CUBE_GL_OPENGL_CALL(name, gl ## name ## ARB)                         \
	/**/
# define _CUBE_GL_OPENGL_WRAP_ARB_RET(name, type)                             \
		_CUBE_GL_OPENGL_CALL_RET(name, gl ## name ## ARB, type)               \
	/**/

		_CUBE_GL_OPENGL_WRAP(Enable);
		_CUBE_GL_OPENGL_WRAP(Disable);
		_CUBE_GL_OPENGL_WRAP(EnableClientState);
		_CUBE_GL_OPENGL_WRAP(DisableClientState);
		_CUBE_GL_OPENGL_WRAP(ClientActiveTexture);

		_CUBE_GL_OPENGL_WRAP_ARB(GenBuffers);
		_CUBE_GL_OPENGL_WRAP_ARB(BindBuffer);
		_CUBE_GL_OPENGL_WRAP_ARB(BufferData);
		_CUBE_GL_OPENGL_WRAP_ARB(BufferSubData);
		_CUBE_GL_OPENGL_WRAP_ARB(DeleteBuffers);


		_CUBE_GL_OPENGL_WRAP(VertexPointer);
		_CUBE_GL_OPENGL_WRAP(ColorPointer);
		_CUBE_GL_OPENGL_WRAP(NormalPointer);
		_CUBE_GL_OPENGL_WRAP(TexCoordPointer);

		_CUBE_GL_OPENGL_WRAP(DrawElements);
		_CUBE_GL_OPENGL_WRAP(DrawArrays);
		_CUBE_GL_OPENGL_WRAP(Clear);
		_CUBE_GL_OPENGL_WRAP(ClearColor);
		_CUBE_GL_OPENGL_WRAP(Viewport);

		_CUBE_GL_OPENGL_WRAP(LoadIdentity);

		//_CUBE_GL_OPENGL_CALL_RET(CreateShader, glCreateShaderObjectARB, GLuint);
		//_CUBE_GL_OPENGL_WRAP(DeleteShader);
		//_CUBE_GL_OPENGL_WRAP_ARB(ShaderSource);
		//_CUBE_GL_OPENGL_WRAP_ARB(CompileShader);
		//_CUBE_GL_OPENGL_WRAP(GetShaderiv);
		//_CUBE_GL_OPENGL_WRAP(GetShaderInfoLog);

		//_CUBE_GL_OPENGL_CALL_RET(CreateProgram, glCreateProgramObjectARB, GLuint);
		//_CUBE_GL_OPENGL_WRAP(DeleteProgram);
		//_CUBE_GL_OPENGL_WRAP_ARB(LinkProgram);
		//_CUBE_GL_OPENGL_WRAP_ARB(ValidateProgram);
		//_CUBE_GL_OPENGL_WRAP_ARB(GetProgramiv);
		//_CUBE_GL_OPENGL_WRAP(GetProgramInfoLog);
		//_CUBE_GL_OPENGL_WRAP(AttachShader);
		//_CUBE_GL_OPENGL_WRAP(DetachShader);
		//_CUBE_GL_OPENGL_CALL(UseProgram, glUseProgramObjectARB);
		//_CUBE_GL_OPENGL_WRAP_ARB_RET(GetUniformLocation, GLint);
		//_CUBE_GL_OPENGL_WRAP_ARB(UniformMatrix4fv);

		_CUBE_GL_OPENGL_WRAP_RET(CreateShader, GLuint);
		_CUBE_GL_OPENGL_WRAP(DeleteShader);
		_CUBE_GL_OPENGL_WRAP(ShaderSource);
		_CUBE_GL_OPENGL_WRAP(CompileShader);
		_CUBE_GL_OPENGL_WRAP(GetShaderiv);
		_CUBE_GL_OPENGL_WRAP(GetShaderInfoLog);

		_CUBE_GL_OPENGL_WRAP_RET(CreateProgram, GLuint);
		_CUBE_GL_OPENGL_WRAP(DeleteProgram);
		_CUBE_GL_OPENGL_WRAP(LinkProgram);
		_CUBE_GL_OPENGL_WRAP(ValidateProgram);
		_CUBE_GL_OPENGL_WRAP(GetProgramiv);
		_CUBE_GL_OPENGL_WRAP(GetProgramInfoLog);
		_CUBE_GL_OPENGL_WRAP(AttachShader);
		_CUBE_GL_OPENGL_WRAP(DetachShader);
		_CUBE_GL_OPENGL_WRAP(UseProgram);
		_CUBE_GL_OPENGL_WRAP_RET(GetUniformLocation, GLint);
		_CUBE_GL_OPENGL_WRAP(UniformMatrix4fv);

# undef _CUBE_GL_OPENGL_WRAP
# undef _CUBE_GL_OPENGL_WRAP_RET
# undef _CUBE_GL_OPENGL_WRAP_ARB
# undef _CUBE_GL_OPENGL_WRAP_RET_ARB
# undef _CUBE_GL_OPENGL_CALL
# undef _CUBE_GL_OPENGL_CALL_RET
# undef _CUBE_GL_OPENGL_LOG
# undef _CUBE_GL_OPENGL_PROTO
# undef _CUBE_GL_OPENGL_PROTO_RET

	public:
		static inline
		GLenum get_draw_mode(renderer::DrawMode value)
		{ return _draw_mode_map[(size_t) value]; }

		static inline
		GLenum get_content_type(renderer::ContentType value)
		{ return _content_type_map[(size_t) value]; }

		static inline
		GLenum get_content_kind(renderer::ContentKind value)
		{ return _content_kind_map[(size_t) value]; }

		static inline
		GLenum get_content_hint(renderer::ContentHint value)
		{ return _content_hint_map[(size_t) value]; }

		static inline
		GLenum get_shader_type(renderer::ShaderType value)
		{ return _shader_type_map[(size_t) value]; }

	private:
		static GLenum _draw_mode_map[(size_t)renderer::DrawMode::_max_value];
		static GLenum _content_type_map[(size_t)renderer::ContentType::_max_value];
		static GLenum _content_kind_map[(size_t)renderer::ContentKind::_max_value];
		static GLenum _content_hint_map[(size_t)renderer::ContentHint::_max_value];
		static GLenum _shader_type_map[(size_t)renderer::ShaderType::_max_value];
	};

}}} // !cube::gl::opengl

#endif
