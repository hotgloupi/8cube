#ifndef  CUBE_GL_OPENGL__OPENGL_HPP
# define CUBE_GL_OPENGL__OPENGL_HPP

# include "Exception.hpp"

# include <cube/debug.hpp>
# include <cube/gl/renderer.hpp>

# include <wrappers/sdl.hpp>
# include <wrappers/opengl.hpp>

# include <etc/log.hpp>

# include <stdexcept>
# include <array>

# define _CUBE_GL_ENUM_HASHABLE(E)                                            \
	namespace std {                                                           \
		template<> struct hash<E> {                                           \
			size_t operator ()(E e) const { return static_cast<size_t>(e); }  \
		};                                                                    \
	}                                                                         \

	_CUBE_GL_ENUM_HASHABLE(cube::gl::renderer::ContentType);
	_CUBE_GL_ENUM_HASHABLE(cube::gl::renderer::ContentHint);

# undef _CUBE_GL_ENUM_HASHABLE

namespace cube { namespace gl { namespace renderer { namespace opengl {

	using namespace ::cube::gl::renderer;

	struct gl
	{

	public:
		enum ThrowPolicy { can_throw, no_throw };
	private:
		template<ThrowPolicy error_policy>
		static void _check_error(char const* function_)
			ETC_NOEXCEPT_IF(error_policy == no_throw);
		ETC_LOG_COMPONENT("cube.gl.renderer.opengl.Proxy");

	public:
# define _CUBE_GL_OPENGL_PROTO(name, type)                                    \
		template<ThrowPolicy error_policy = can_throw, typename... T>         \
		static inline                                                         \
		type name(T... values) ETC_NOEXCEPT_IF(error_policy == no_throw)      \
	/**/

// values... is defined in the proto
# define _CUBE_GL_OPENGL_LOG(gl_name)                                         \
		ETC_LOG.debug(#gl_name, '(', values..., ')')                          \
	/**/

# define _CUBE_GL_OPENGL_CALL(name, gl_name)                                  \
		_CUBE_GL_OPENGL_PROTO(name, void)                                     \
		{                                                                     \
			_CUBE_GL_OPENGL_LOG(gl_name);                                     \
			CUBE_DEBUG_PERFORMANCE_SECTION("cube.OpenGLRenderer"); \
			::gl_name(values...);                                             \
			_check_error<error_policy>(#gl_name);                              \
		}
	/**/

# define _CUBE_GL_OPENGL_CALL_RET(name, gl_name, type)                        \
		_CUBE_GL_OPENGL_PROTO(name, type)                                     \
		{                                                                     \
			_CUBE_GL_OPENGL_LOG(gl_name);                                     \
			CUBE_DEBUG_PERFORMANCE_SECTION("cube.OpenGLRenderer"); \
			type ret = ::gl_name(values...);                                  \
			_check_error<error_policy>(#gl_name);                              \
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

		_CUBE_GL_OPENGL_WRAP(ActiveTexture);
		_CUBE_GL_OPENGL_WRAP(AttachShader);
		_CUBE_GL_OPENGL_WRAP(BindAttribLocation);
		_CUBE_GL_OPENGL_WRAP(BindFragDataLocation);
		_CUBE_GL_OPENGL_WRAP(BindTexture);
		_CUBE_GL_OPENGL_WRAP(Clear);
		_CUBE_GL_OPENGL_WRAP(ClearColor);
		_CUBE_GL_OPENGL_WRAP(ClientActiveTexture);
		_CUBE_GL_OPENGL_WRAP(ColorPointer);
		_CUBE_GL_OPENGL_WRAP(CompileShader);
		_CUBE_GL_OPENGL_WRAP(DeleteProgram);
		_CUBE_GL_OPENGL_WRAP(DeleteShader);
		_CUBE_GL_OPENGL_WRAP(DeleteTextures);
		_CUBE_GL_OPENGL_WRAP(DetachShader);
		_CUBE_GL_OPENGL_WRAP(Disable);
		_CUBE_GL_OPENGL_WRAP(DisableClientState);
		_CUBE_GL_OPENGL_WRAP(DisableVertexAttribArray);
		_CUBE_GL_OPENGL_WRAP(DrawArrays);
		_CUBE_GL_OPENGL_WRAP(DrawElements);
		_CUBE_GL_OPENGL_WRAP(Enable);
		_CUBE_GL_OPENGL_WRAP(EnableClientState);
		_CUBE_GL_OPENGL_WRAP(EnableVertexAttribArray);
		_CUBE_GL_OPENGL_WRAP(GenTextures);
		_CUBE_GL_OPENGL_WRAP(GenerateMipmap);
		_CUBE_GL_OPENGL_WRAP(GetActiveUniform);
		_CUBE_GL_OPENGL_WRAP(GetProgramInfoLog);
		_CUBE_GL_OPENGL_WRAP(GetProgramiv);
		_CUBE_GL_OPENGL_WRAP(GetShaderInfoLog);
		_CUBE_GL_OPENGL_WRAP(GetShaderiv);
		_CUBE_GL_OPENGL_WRAP(GetTexImage);
		_CUBE_GL_OPENGL_WRAP(LinkProgram);
		_CUBE_GL_OPENGL_WRAP(LoadIdentity);
		_CUBE_GL_OPENGL_WRAP(NormalPointer);
		_CUBE_GL_OPENGL_WRAP(ShaderSource);
		_CUBE_GL_OPENGL_WRAP(TexCoordPointer);
		_CUBE_GL_OPENGL_WRAP(TexImage2D);
		_CUBE_GL_OPENGL_WRAP(TexParameteri);
		_CUBE_GL_OPENGL_WRAP(TexStorage2D);
		_CUBE_GL_OPENGL_WRAP(TexSubImage2D);
		_CUBE_GL_OPENGL_WRAP(Uniform1f);
		_CUBE_GL_OPENGL_WRAP(Uniform1i);
		_CUBE_GL_OPENGL_WRAP(Uniform3fv);
		_CUBE_GL_OPENGL_WRAP(UniformMatrix3fv);
		_CUBE_GL_OPENGL_WRAP(UniformMatrix4fv);
		_CUBE_GL_OPENGL_WRAP(UseProgram);
		_CUBE_GL_OPENGL_WRAP(ValidateProgram);
		_CUBE_GL_OPENGL_WRAP(VertexPointer);
		_CUBE_GL_OPENGL_WRAP(VertexAttribPointer);
		_CUBE_GL_OPENGL_WRAP(Viewport);
		_CUBE_GL_OPENGL_WRAP_ARB(BindBuffer);
		_CUBE_GL_OPENGL_WRAP_ARB(BufferData);
		_CUBE_GL_OPENGL_WRAP_ARB(BufferSubData);
		_CUBE_GL_OPENGL_WRAP_ARB(DeleteBuffers);
		_CUBE_GL_OPENGL_WRAP_ARB(GenBuffers);
		_CUBE_GL_OPENGL_WRAP_RET(CreateProgram, GLuint);
		_CUBE_GL_OPENGL_WRAP_RET(CreateShader, GLuint);
		_CUBE_GL_OPENGL_WRAP_RET(GetUniformLocation, GLint);

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
		GLenum get_content_packing(renderer::ContentPacking value)
		{ return _content_packing_map[(size_t) value]; }

		static
		GLenum get_content_kind(renderer::ContentKind value);

		static
		GLenum get_content_hint(renderer::ContentHint value)
		{ return _content_hint_map[(size_t) value]; }

		static inline
		GLenum get_shader_type(renderer::ShaderType value)
		{ return _shader_type_map[(size_t) value]; }

		static inline
		GLenum get_pixel_format(renderer::PixelFormat value)
		{ return _pixel_format_map[(size_t) value]; }

		static
		renderer::PixelFormat to_pixel_format(GLenum value) ETC_NOEXCEPT;
		static
		renderer::ContentPacking to_content_packing(GLenum value) ETC_NOEXCEPT;
	public:
		static size_t const _draw_modes = (size_t) renderer::DrawMode::_max_value;
		static std::array<GLenum, _draw_modes> _draw_mode_map;

		static size_t const _content_types = (size_t)renderer::ContentType::_max_value;
		static std::array<GLenum, _content_types> _content_type_map;

		static size_t const _content_packings = (size_t)renderer::ContentPacking::_max_value;
		static std::array<GLenum, _content_packings> _content_packing_map;


		static size_t const _content_hints = (size_t)renderer::ContentHint::_max_value;
		static std::array<GLenum, _content_hints> _content_hint_map;

		static size_t const _shader_types = (size_t)renderer::ShaderType::_max_value;
		static std::array<GLenum, _shader_types> _shader_type_map;

		static size_t const _pixel_formats = (size_t)renderer::PixelFormat::_max_value;
		static std::array<GLenum, _pixel_formats> _pixel_format_map;


	public:
		struct SubVBO;
    	template<bool> struct VBO;
	};

}}}} // !cube::gl::opengl


// old api compat, here just in case

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

#endif
