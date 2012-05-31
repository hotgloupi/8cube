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

			static std::unordered_map<ContentType, int> content_type_map;
			static std::unordered_map<ContentKind, int> content_kind_map;
			static std::unordered_map<ContentHint, int> content_hint_map;
		};

		std::unordered_map<ContentType, int> gl::content_type_map = {
			{ContentType::int8,  GL_BYTE},
			{ContentType::uint8, GL_UNSIGNED_BYTE},
			{ContentType::int16, GL_SHORT},
			{ContentType::uint16, GL_UNSIGNED_SHORT},
			{ContentType::int32, GL_INT},
			{ContentType::uint32, GL_UNSIGNED_INT},
			{ContentType::float32, GL_FLOAT},
		};

		std::unordered_map<ContentKind, int> content_kind_map  = {
		};


		std::unordered_map<ContentHint, int> content_hint_map = {
			{ContentHint::stream_content, GL_STREAM_DRAW},
			{ContentHint::static_content, GL_STATIC_DRAW},
			{ContentHint::dynamic_content, GL_DYNAMIC_DRAW},
		};

	} // !anonymous

}}} // !cube::gl::opengl

#endif
