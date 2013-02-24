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
		, near_distance{ndist}
		, far_distance{fdist}
		, near_size{_plane_size(fov, ratio, ndist)}
		, far_size{_plane_size(fov, ratio, fdist)}
		, _planes{}
	{}

	template<typename T>
	bool
	Frustum<T>::contains(vec3 const& point) const
	{
		for (plane_t const& plane: _planes)
			if (plane.distance(point) > 0)
				return false;
		return true;
	}

	template<typename T>
	vector::Vector2<T>
	Frustum<T>::_plane_size(float const fov,
	                        float const ratio,
	                        T const distance)
	{
		T const height = glm::tan(glm::radians(fov) / 2.0f) * (T{2} * distance);
		return vec2{height * ratio, height};
	}

	template<typename T>
	void
	Frustum<T>::update(vec3 const& position,
	                   vec3f const& direction,
	                   vec3f const& up)
	{
		auto const near_center = position + vec3{
			this->near_distance * direction.x,
			this->near_distance * direction.y,
			this->near_distance * direction.z,
		};
		auto const far_center = position + vec3{
			this->far_distance * direction.x,
			this->far_distance * direction.y,
			this->far_distance * direction.z,
		};
		auto const right = vector::cross(direction, up);

		// near points
		auto half_near_size = this->near_size / T{2};
		if (half_near_size.x == 0)
			half_near_size.x = 1;
		if (half_near_size.y == 0)
			half_near_size.y = 1;

		vec3 const half_near_up{
			up.x * half_near_size.y,
			up.y * half_near_size.y,
			up.z * half_near_size.y,
		};
		vec3 const half_near_right{
			right.x * half_near_size.x,
			right.y * half_near_size.x,
			right.z * half_near_size.x,
		};
		vec3 const near_tl{near_center + half_near_up - half_near_right};
		vec3 const near_tr{near_center + half_near_up + half_near_right};
		vec3 const near_bl{near_center - half_near_up - half_near_right};
		vec3 const near_br{near_center - half_near_up + half_near_right};

		// far points
		auto const half_far_size = this->far_size / T{2};
		vec3 const half_far_up{
			up.x * half_far_size.y,
			up.y * half_far_size.y,
			up.z * half_far_size.y,
		};
		vec3 const half_far_right{
			right.x * half_far_size.x,
			right.y * half_far_size.x,
			right.z * half_far_size.x,
		};
		vec3 const far_tl{far_center + half_far_up - half_far_right};
		vec3 const far_tr{far_center + half_far_up + half_far_right};
		vec3 const far_bl{far_center - half_far_up - half_far_right};
		vec3 const far_br{far_center - half_far_up + half_far_right};

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
