#include "frustum.hpp"

#include <etc/print.hpp>

#include <glm/core/func_trigonometric.hpp>

namespace cube { namespace gl { namespace frustum {

	template<typename T>
	Frustum<T>::Frustum(float const fov,
	                    float const ratio,
	                    T const ndist,
	                    T const fdist)
		: fov{fov}
		, ratio{ratio}
		, near_distance{static_cast<double>(ndist)}
		, far_distance{static_cast<double>(fdist)}
		, near_size{_plane_size(fov, ratio, ndist)}
		, far_size{_plane_size(fov, ratio, fdist)}
		, _planes{}
	{}

	template<typename T>
	bool
	Frustum<T>::contains(vec3 const& point) const
	{
		vec3d p{point.x, point.y, point.z};
		for (plane_t const& plane: _planes)
			if (plane.distance(p) > 0)
				return false;
		return true;
	}

	template<typename T>
	bool
	Frustum<T>::intersect(sphere_t const& sphere) const
	{
		double const radius = sphere.radius;
		vec3d const center{
			sphere.center.x,
			sphere.center.y,
			sphere.center.z
		};
		bool ret = false;
		for (plane_t const& plane: _planes)
		{
			double const d = plane.distance(center);
			if (d > radius)
				return false;
			if (!ret && d >= -radius)
				ret = true;
		}
		return ret;
	}

	template<typename T>
	vector::Vector2d
	Frustum<T>::_plane_size(float const fov,
	                        float const ratio,
	                        T const distance)
	{
		double const height =
			glm::tan(glm::radians(fov) / 2.0) * (2.0 * distance);
		return vec2d{height * ratio, height};
	}

	template<typename T>
	void
	Frustum<T>::update(vec3 const& position_,
	                   vec3f const& direction_,
	                   vec3f const& up_)
	{
		vec3d pos{position_.x, position_.y, position_.z};
		vec3d dir = gl::vector::normalize(
			vec3d{direction_.x, direction_.y, direction_.z}
		);
		vec3d up = gl::vector::normalize(
			vec3d{up_.x, up_.y, up_.z}
		);

		auto const near_center = pos + dir * this->near_distance;
		auto const far_center = pos + dir * this->far_distance;
		auto const right = vector::cross(dir, up);

		// near points
		auto half_near_size = this->near_size / 2.0;
		if (half_near_size.x == 0)
			half_near_size.x = 1;
		if (half_near_size.y == 0)
			half_near_size.y = 1;

		vec3d const half_near_up{
			up.x * half_near_size.y,
			up.y * half_near_size.y,
			up.z * half_near_size.y,
		};
		vec3d const half_near_right{
			right.x * half_near_size.x,
			right.y * half_near_size.x,
			right.z * half_near_size.x,
		};
		vec3d const near_tl{near_center + half_near_up - half_near_right};
		vec3d const near_tr{near_center + half_near_up + half_near_right};
		vec3d const near_bl{near_center - half_near_up - half_near_right};
		vec3d const near_br{near_center - half_near_up + half_near_right};

		// far points
		auto const half_far_size = this->far_size / 2.0;
		vec3d const half_far_up{
			up.x * half_far_size.y,
			up.y * half_far_size.y,
			up.z * half_far_size.y,
		};
		vec3d const half_far_right{
			right.x * half_far_size.x,
			right.y * half_far_size.x,
			right.z * half_far_size.x,
		};
		vec3d const far_tl{far_center + half_far_up - half_far_right};
		vec3d const far_tr{far_center + half_far_up + half_far_right};
		vec3d const far_bl{far_center - half_far_up - half_far_right};
		vec3d const far_br{far_center - half_far_up + half_far_right};

		// XXX normals point to outside of the frustum
		this->_plane(PlanePosition::top, {near_tl, near_tr, far_tl});
		this->_plane(PlanePosition::bottom, {near_br, near_bl, far_bl});
		this->_plane(PlanePosition::left, {near_bl, near_tl, far_bl});
		this->_plane(PlanePosition::right, {near_tr, near_br, far_br});
		this->_plane(PlanePosition::near, {near_br, near_tr, near_bl});
		this->_plane(PlanePosition::far, {far_bl, far_tl, far_tr});
	}

	template struct Frustum<float>;
	template struct Frustum<double>;
	template struct Frustum<int32_t>;
	//template struct Frustum<uint32_t>;
	template struct Frustum<int64_t>;
	//template struct Frustum<uint64_t>;

}}}
