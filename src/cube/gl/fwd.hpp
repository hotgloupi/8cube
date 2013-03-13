#ifndef  CUBE_GL_FWD_HH
# define CUBE_GL_FWD_HH

# include <boost/config.hpp>

namespace glm { namespace detail {

	template<typename T> struct tvec2;
	template<typename T> struct tvec3;
	template<typename T> struct tvec4;
	template<typename T> struct tmat4x4;

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

	namespace matrix {

# ifdef BOOST_NO_CXX11_TEMPLATE_ALIASES
		template<typename T>
		struct Matrix44
			: public ::glm::detail::tmat4x4<T>
		{};
# else
		template<typename T>
		using Matrix44 = ::glm::detail::tmat4x4<T>;
# endif

		typedef Matrix44<float>    Matrix44f;
		typedef Matrix44<double>   Matrix44d;

	}

	namespace mesh {

		class Mesh;

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
		template<typename T> using Vector4 = glm::detail::tvec4<T>;

		typedef Vector2<float> Vector2f;
		typedef Vector3<float> Vector3f;
		typedef Vector4<float> Vector4f;

	}

	namespace viewport {

		struct Viewport;

	}

}} // !cube::gl

#endif

