#ifndef  CUBE_GL_FWD_HH
# define CUBE_GL_FWD_HH

# include <cube/api.hpp>

# include <etc/memory.hpp>
# include <etc/types.hpp>

# include <glm/fwd.hpp>

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

	namespace material {

		class CUBE_API Material;
		typedef std::shared_ptr<Material> MaterialPtr;

	}

	namespace matrix {

		template<typename T>
		using Matrix33 = ::glm::detail::tmat3x3<T, glm::precision::highp>;
		typedef Matrix33<float>    mat3f;
		typedef Matrix33<double>   mat3d;


		template<typename T>
		using Matrix44 = ::glm::detail::tmat4x4<T, glm::precision::highp>;
		typedef Matrix44<float>    mat4f;
		typedef Matrix44<double>   mat4d;

	}

	namespace mesh {

		class CUBE_API Mesh;
		typedef std::shared_ptr<Mesh> MeshPtr;

	}

	namespace rectangle {

		template<typename T> struct Rectangle;
		typedef Rectangle<float> Rectanglef;
		typedef Rectangle<uint32_t> Rectangleu;
		typedef Rectangle<int32_t> Rectanglei;

	}

	namespace surface {

		class Surface;

	}

	namespace text {

		class CUBE_API Text;

	}

	namespace vector {

		template<typename T>
			using Vector2 = glm::detail::tvec2<T, glm::precision::highp>;
		template<typename T>
			using Vector3 = glm::detail::tvec3<T, glm::precision::highp>;
		template<typename T>
			using Vector4 = glm::detail::tvec4<T, glm::precision::highp>;

		/// Define {vec}{2,3,4}{f,d,i,u,il,ul} aliases.
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

