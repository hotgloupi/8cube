#ifndef  CUBE_GL_OPENGL__OPENGL_HPP
# define CUBE_GL_OPENGL__OPENGL_HPP

# include <unordered_map>

# include <boost/type_traits.hpp>

# include <wrappers/sdl.hpp>
# include <wrappers/opengl.hpp>

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

	namespace {

		struct gl
		{
# define _CUBE_GL_OPENGL_WRAP(__name__)                                       \
			typedef                                                           \
				boost::function_traits<decltype(glEnable)>::result_type       \
				__name__ ## _result_type;                                     \
			template<typename... T>                                           \
			static inline __name__ ## _result_type                            \
			__name__(T... values)                                             \
			{                                                                 \
				return ::gl ## __name__(values...);                           \
			}                                                                 \

			_CUBE_GL_OPENGL_WRAP(Enable);
			_CUBE_GL_OPENGL_WRAP(Disable);
			_CUBE_GL_OPENGL_WRAP(EnableClientState);
			_CUBE_GL_OPENGL_WRAP(DisableClientState);
			_CUBE_GL_OPENGL_WRAP(ClientActiveTextureARB);

			_CUBE_GL_OPENGL_WRAP(GenBuffersARB);
			_CUBE_GL_OPENGL_WRAP(BindBufferARB);
			_CUBE_GL_OPENGL_WRAP(BufferDataARB);
			_CUBE_GL_OPENGL_WRAP(BufferSubDataARB);
			_CUBE_GL_OPENGL_WRAP(DeleteBuffersARB);


			_CUBE_GL_OPENGL_WRAP(VertexPointer);
			_CUBE_GL_OPENGL_WRAP(ColorPointer);
			_CUBE_GL_OPENGL_WRAP(NormalPointer);
			_CUBE_GL_OPENGL_WRAP(TexCoordPointer);


# undef _CUBE_GL_OPENGL_WRAP

		public:
			static inline int get_content_type(ContentType value) { return _content_type_map[(size_t) value]; }
			static inline int get_content_kind(ContentKind value) { return _content_kind_map[(size_t) value]; }
			static inline int get_content_hint(ContentHint value) { return _content_hint_map[(size_t) value]; }
		private:
			static int _content_type_map[(size_t)ContentType::_max_value];
			static int _content_kind_map[(size_t)ContentKind::_max_value];
			static int _content_hint_map[(size_t)ContentHint::_max_value];
		};

		int gl::_content_type_map[(size_t)ContentType::_max_value] = {
			GL_BYTE,    GL_UNSIGNED_BYTE,   GL_SHORT,   GL_UNSIGNED_SHORT,
			GL_INT,     GL_UNSIGNED_INT,    GL_FLOAT
		};

		int gl::_content_kind_map[(size_t)ContentKind::_max_value]  = {
			GL_VERTEX_ARRAY, GL_COLOR_ARRAY, GL_NORMAL_ARRAY,
			GL_TEXTURE0_ARB, GL_TEXTURE1_ARB, GL_TEXTURE2_ARB,
		};

		int gl::_content_hint_map[(size_t)ContentHint::_max_value] = {
			GL_STREAM_DRAW, GL_STATIC_DRAW, GL_DYNAMIC_DRAW,
		};

	} // !anonymous

}}} // !cube::gl::opengl

#endif
