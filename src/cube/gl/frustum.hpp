#ifndef  CUBE_GL_FRUSTUM_HPP
# define CUBE_GL_FRUSTUM_HPP

# include "vector.hpp"
# include "plane.hpp"
# include "sphere.hpp"

namespace cube { namespace gl { namespace frustum {

	enum class PlanePosition
	{
		top,
		bottom,
		left,
		right,
		near,
		far,
	};

	template<typename T>
	struct Frustum
	{
	public:
		typedef vector::Vector2<T>  vec2;
		typedef vector::Vector3<T>  vec3;
		typedef vector::Vector3f    vec3f;
		typedef plane::Plane<T>     plane_t;
		typedef sphere::Sphere<T>   sphere_t;

	public:
		float const fov;
		float const ratio;
		T const     near_distance;
		T const     far_distance;
		vec2 const  near_size;
		vec2 const  far_size;
	private:
		plane_t     _planes[6];

	public:
		Frustum(float const fov,
		        float const ratio,
		        T const ndist,
		        T const fdist);

		void update(vec3 const& position,
		            vec3f const& front,
		            vec3f const& up);

		inline
		plane_t const& plane(PlanePosition const pos)
		{
			return _planes[static_cast<etc::size_type>(pos)];
		}

		/**
		 * @brief Check whether or not a point is in the frustum.
		 */
		bool contains(vec3 const& point) const;

		bool intersect(sphere_t const& sphere) const;

	private:
		inline
		void _plane(PlanePosition const pos, plane_t const& p)
		{
			_planes[static_cast<etc::size_type>(pos)] = p;
		}

	private:
		static
		vector::Vector2<T> _plane_size(float const fov,
		                               float const ratio,
		                               T const distance);
	};

	typedef Frustum<float>    Frustumf;
	typedef Frustum<double>   Frustumd;
	typedef Frustum<int32_t>  Frustumi;
	//typedef Frustum<uint32_t> Frustumu;
	typedef Frustum<int64_t>  Frustumil;
	//typedef Frustum<uint64_t> Frustumul;

}}}

#endif

