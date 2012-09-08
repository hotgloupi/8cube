#ifndef  CUBE_GL_FWD_HH
# define CUBE_GL_FWD_HH

# include "renderer/fwd.hpp"

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

		template<typename T> struct Vector2;
		template<typename T> struct Vector3;

		typedef Vector2<float> Vector2f;
		typedef Vector3<float> Vector3f;

	}

	namespace viewport {

		struct Viewport;

	}

}} // !cube::gl

#endif

