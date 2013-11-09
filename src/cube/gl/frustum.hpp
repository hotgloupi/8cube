#ifndef  CUBE_GL_FRUSTUM_HPP
# define CUBE_GL_FRUSTUM_HPP

# include "fwd.hpp"

# include "vector.hpp"
# include "plane.hpp"
# include "renderer/fwd.hpp"
# include "sphere.hpp"

# include <cube/api.hpp>
# include <cube/units/angle.hpp>

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
	struct CUBE_API Frustum
	{
	public:
		typedef vector::Vector2d      vec2d;
		typedef vector::Vector3<T>    vec3;
		typedef vector::Vector3f      vec3f;
		typedef vector::Vector3d      vec3d;
		typedef plane::Planed         plane_t;
		typedef sphere::Sphere<T>     sphere_t;

	public:
		units::Angle const   fov;
		float const   ratio;
		double const  near_distance;
		double const  far_distance;
		vec2d const   near_size;
		vec2d const   far_size;
		struct Quad
		{
			vec3d tl;
			vec3d tr;
			vec3d br;
			vec3d bl;
		};

	public:
		Quad near;
		Quad far;
	private:
		plane_t     _planes[6];

	public:
		Frustum(units::Angle const fov,
		        float const ratio,
		        T const ndist,
		        T const fdist) noexcept;

		void update(vec3f const& front,
		            vec3f const& up) noexcept;

		inline
		plane_t const& plane(PlanePosition const pos) noexcept
		{ return _planes[static_cast<etc::size_type>(pos)]; }

		/**
		 * @brief Retreive a mesh representing the frustum according to the
		 * last update.
		 */
		mesh::Mesh mesh() const;

		/**
		 * @brief Drawable of the frustum.
		 */
		std::unique_ptr<renderer::Drawable>
		view(renderer::Renderer& renderer) const;

		/**
		 * @brief Check whether or not a point is in the frustum.
		 */
		bool contains(vec3 const& point) const noexcept;

		bool intersects(sphere_t const& sphere) const noexcept;

	private:
		CUBE_API_INTERNAL
		inline
		void _plane(PlanePosition const pos, plane_t const& p) noexcept
		{ _planes[static_cast<etc::size_type>(pos)] = p; }

	private:
		CUBE_API_INTERNAL
		static
		vec2d _plane_size(units::Angle const fov,
		                  float const ratio,
		                  T const distance) noexcept;
	};

}}}

#endif

