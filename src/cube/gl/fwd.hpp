#ifndef  CUBE_GL_FWD_HH
# define CUBE_GL_FWD_HH

# include <cube/api.hpp>

# include <etc/types.hpp>

namespace glm { namespace detail {

	template<typename T> struct tvec2;
	template<typename T> struct tvec3;
	template<typename T> struct tvec4;
	template<typename T> struct tmat4x4;

}}

namespace cube { namespace gl {

	namespace color {

		template<typename T> struct CUBE_API Color3;
		template<typename T> struct CUBE_API Color4;

		typedef Color3<float> Color3f;
		typedef Color4<float> Color4f;

	}

	namespace font {

		class CUBE_API Font;

	}

	namespace frustum {

		template<typename T>
		struct CUBE_API Frustum;

		typedef Frustum<float>    Frustumf;
		typedef Frustum<double>   Frustumd;
		typedef Frustum<int32_t>  Frustumi;
		//typedef Frustum<uint32_t> Frustumu;
		typedef Frustum<int64_t>  Frustumil;
		//typedef Frustum<uint64_t> Frustumul;

	}


	namespace matrix {

		template<typename T>
		using Matrix44 = ::glm::detail::tmat4x4<T>;

		typedef Matrix44<float>    Matrix44f;
		typedef Matrix44<double>   Matrix44d;

	}

	namespace mesh {

		class CUBE_API Mesh;
		typedef std::unique_ptr<Mesh> MeshPtr;

	}

	namespace rectangle {

		template<typename T> struct Rectangle2;
		typedef Rectangle2<float> Rectangle2f;

	}

	namespace surface {

		class Surface;

	}

	namespace text {

		class CUBE_API Text;

	}

	namespace vector {

		template<typename T> using Vector2 = glm::detail::tvec2<T>;
		template<typename T> using Vector3 = glm::detail::tvec3<T>;
		template<typename T> using Vector4 = glm::detail::tvec4<T>;

		/// Define {Vector,vec}{2,3,4}{f,d,i,u,il,ul} aliases.
# define _CUBE_GL_VECTOR_DEF(__arity, __type, __suffix)                       \
		typedef Vector ## __arity<__type> Vector ## __arity ## __suffix;      \
		typedef Vector ## __arity<__type> vec ## __arity ## __suffix;         \
/**/
		_CUBE_GL_VECTOR_DEF(2, float, f);
		_CUBE_GL_VECTOR_DEF(2, double, d);
		_CUBE_GL_VECTOR_DEF(2, int32_t, i);
		_CUBE_GL_VECTOR_DEF(2, uint32_t, u);
		_CUBE_GL_VECTOR_DEF(2, int64_t, il);
		_CUBE_GL_VECTOR_DEF(2, uint64_t, ul);

		_CUBE_GL_VECTOR_DEF(3, float, f);
		_CUBE_GL_VECTOR_DEF(3, double, d);
		_CUBE_GL_VECTOR_DEF(3, int32_t, i);
		_CUBE_GL_VECTOR_DEF(3, uint32_t, u);
		_CUBE_GL_VECTOR_DEF(3, int64_t, il);
		_CUBE_GL_VECTOR_DEF(3, uint64_t, ul);

		_CUBE_GL_VECTOR_DEF(4, float, f);
		_CUBE_GL_VECTOR_DEF(4, double, d);
		_CUBE_GL_VECTOR_DEF(4, int32_t, i);
		_CUBE_GL_VECTOR_DEF(4, uint32_t, u);
		_CUBE_GL_VECTOR_DEF(4, int64_t, il);
		_CUBE_GL_VECTOR_DEF(4, uint64_t, ul);
# undef _CUBE_GL_VECTOR_DEF

	}

	namespace viewport {

		struct CUBE_API Viewport;

	}

}} // !cube::gl

#endif

