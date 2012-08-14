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
	public:
# define _CUBE_GL_OPENGL_WRAP(__name__)                                       \
		typedef                                                               \
			boost::function_traits<decltype(glEnable)>::result_type           \
			__name__ ## _result_type;                                         \
		template<typename... T>                                               \
		static inline __name__ ## _result_type                                \
		__name__(T... values)                                                 \
		{                                                                     \
			etc::log::debug(__PRETTY_FUNCTION__, values...);                  \
			return ::gl ## __name__(values...);                               \
		}                                                                     \

		_CUBE_GL_OPENGL_WRAP(Enable);
		_CUBE_GL_OPENGL_WRAP(Disable);
		_CUBE_GL_OPENGL_WRAP(EnableClientState);
		_CUBE_GL_OPENGL_WRAP(DisableClientState);
		_CUBE_GL_OPENGL_WRAP(ClientActiveTexture);

		_CUBE_GL_OPENGL_WRAP(GenBuffers);
		_CUBE_GL_OPENGL_WRAP(BindBuffer);
		_CUBE_GL_OPENGL_WRAP(BufferData);
		_CUBE_GL_OPENGL_WRAP(BufferSubData);
		_CUBE_GL_OPENGL_WRAP(DeleteBuffers);


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
