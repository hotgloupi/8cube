#ifndef  CUBE_GL_FWD_HH
# define CUBE_GL_FWD_HH

# include "renderer/fwd.hpp"

namespace glm { namespace detail {

	template<typename T> struct tvec2;
	template<typename T> struct tvec3;

}}

namespace cube { namespace gl {

	namespace color {

		template<typename T> struct Color3;
		template<typename T> struct Color4;

		typedef Color3<float> Color3f;
		typedef Color4<float> Color4f;

	}

	namespace font {

		class Font;

	}

	namespace rectangle {

		template<typename T> struct Rectangle2;
		typedef Rectangle2<float> Rectangle2f;

	}

	namespace text {

		class Text;

	}

	namespace vector {

		template<typename T> using Vector2 = glm::detail::tvec2<T>;
		template<typename T> using Vector3 = glm::detail::tvec3<T>;

		typedef Vector2<float> Vector2f;
		typedef Vector3<float> Vector3f;

	}

	namespace viewport {

		struct Viewport;

	}

}} // !cube::gl

#endif

