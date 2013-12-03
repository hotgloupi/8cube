#ifndef  CUBE_GL_SPHERE_HPP
# define CUBE_GL_SPHERE_HPP

# include "vector.hpp"

# include <cube/api.hpp>

namespace cube { namespace gl { namespace sphere {

	template<typename T>
	struct CUBE_API Sphere
	{
	public:
		typedef vector::Vector3<T> vec3;

	public:
		vec3 const  center;
		T const     radius;

	public:
		Sphere(vec3 const& center, T const radius)
			: center(center)
			, radius{radius}
		{}
	};

	typedef Sphere<float>    Spheref;
	typedef Sphere<double>   Sphered;
	typedef Sphere<int32_t>  Spherei;
	typedef Sphere<uint32_t> Sphereu;
	typedef Sphere<int64_t>  Sphereil;
	typedef Sphere<uint64_t> Sphereul;

	template<typename T>
	CUBE_API
	std::ostream& operator <<(std::ostream& out, Sphere<T> const& sphere);

}}}

#endif

