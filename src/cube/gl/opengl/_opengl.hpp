#ifndef  CUBE_GL_OPENGL__OPENGL_HPP
# define CUBE_GL_OPENGL__OPENGL_HPP

# include <stdexcept>

# include <boost/type_traits.hpp>

# include <wrappers/sdl.hpp>
# include <wrappers/opengl.hpp>

# include <etc/log.hpp>

# include <cube/gl/renderer.hpp>

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
# define _CUBE_GL_OPENGL_PROTO_RET(name, gl_name)                             \
		typedef                                                               \
			boost::function_traits<decltype(&gl_name)>::result_type           \
			gl_name ## _result_type;                                          \
		template<typename... T>                                               \
		static inline gl_name ## _result_type                                 \
		name(T... values)                                                     \
	/**/

# define _CUBE_GL_OPENGL_PROTO(name, gl_name)                                 \
		template<typename... T>                                               \
		static inline void                                                    \
		name(T... values)                                                     \
	/**/

// values... is defined in the proto
# define _CUBE_GL_OPENGL_LOG(gl_name)                                         \
		etc::log::logger("cube.gl.opengl._opengl").debug(                     \
			#gl_name, '(', values..., ')'                                     \
		);                                                                    \
	/**/

# define _CUBE_GL_OPENGL_CALL(name, gl_name)                                  \
		_CUBE_GL_OPENGL_PROTO(name, gl_name)                                  \
		{                                                                     \
			_CUBE_GL_OPENGL_LOG(gl_name)                                      \
			::gl_name(values...);                                             \
			_check_error(#gl_name);                                           \
		}
	/**/

# define _CUBE_GL_OPENGL_CALL_RET(name, gl_name)                              \
		_CUBE_GL_OPENGL_PROTO_RET(name, gl_name)                              \
		{                                                                     \
			_CUBE_GL_OPENGL_LOG(gl_name)                                      \
			gl_name ## _result_type ret = ::gl_name(values...);               \
			_check_error(#gl_name);                                           \
			return ret;                                                       \
		}
	/**/

# define _CUBE_GL_OPENGL_WRAP(name)                                           \
		_CUBE_GL_OPENGL_CALL(name, gl ## name)                                \
	/**/
# define _CUBE_GL_OPENGL_WRAP_RET(name)                                       \
		_CUBE_GL_OPENGL_CALL_RET(name, gl ## name)                            \
	/**/

# define _CUBE_GL_OPENGL_WRAP_ARB(name)                                       \
		_CUBE_GL_OPENGL_CALL(name, gl ## name ## ARB)                         \
	/**/
# define _CUBE_GL_OPENGL_WRAP_ARB_RET(name)                                   \
		_CUBE_GL_OPENGL_CALL_RET(name, gl ## name ## ARB)                     \
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
		template<bool is_indices> struct VBO;
		struct SubVBO;

	public:
		static inline int get_draw_mode(DrawMode value)
			{ return _draw_mode_map[(size_t) value]; }
		static inline int get_content_type(ContentType value)
			{ return _content_type_map[(size_t) value]; }
		static inline int get_content_kind(ContentKind value)
			{ return _content_kind_map[(size_t) value]; }
		static inline int get_content_hint(ContentHint value)
			{ return _content_hint_map[(size_t) value]; }
	private:
		static GLenum _draw_mode_map[(size_t)DrawMode::_max_value];
		static GLenum _content_type_map[(size_t)ContentType::_max_value];
		static GLenum _content_kind_map[(size_t)ContentKind::_max_value];
		static GLenum _content_hint_map[(size_t)ContentHint::_max_value];

	};

}}} // !cube::gl::opengl

#endif
