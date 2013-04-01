#include "frustum.hpp"

#include "mesh.hpp"
#include "renderer/Drawable.hpp"

#include <etc/print.hpp>

#include <glm/core/func_trigonometric.hpp>

namespace cube { namespace gl { namespace frustum {

	template<typename T>
	Frustum<T>::Frustum(units::Angle const fov,
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
	Frustum<T>::intersects(sphere_t const& sphere) const
	{
		double const radius = sphere.radius;
		vec3d const center{
			sphere.center.x,
			sphere.center.y,
			sphere.center.z
		};
		for (plane_t const& plane: _planes)
		{
			if (plane.distance(center) > radius)
			{
				return false;
			}
		}
		return true;
	}

	template<typename T>
	vector::Vector2d
	Frustum<T>::_plane_size(units::Angle const fov,
	                        float const ratio,
	                        T const distance)
	{
		double const height =
			glm::tan(units::rad_value(fov) / 2.0) * (2.0 * distance);
		return vec2d{height * ratio, height};
	}

	template<typename T>
	void
	Frustum<T>::update(vec3f const& direction_,
	                   vec3f const& up_)
	{
		vec3d dir = gl::vector::normalize(
			vec3d{direction_.x, direction_.y, direction_.z}
		);
		vec3d up = gl::vector::normalize(
			vec3d{up_.x, up_.y, up_.z}
		);

		auto const near_center = dir * this->near_distance;
		auto const far_center = dir * this->far_distance;
		auto const right = gl::vector::normalize(vector::cross(dir, up));

		// near points
		auto half_near_size = this->near_size / 2.0;
		vec3d const half_near_up = up * half_near_size.y;
		vec3d const half_near_right = right * half_near_size.x;
		this->near.tl = near_center + half_near_up - half_near_right;
		this->near.tr = near_center + half_near_up + half_near_right;
		this->near.bl = near_center - half_near_up - half_near_right;
		this->near.br = near_center - half_near_up + half_near_right;


		// far points
		auto const half_far_size = this->far_size / 2.0;
		vec3d const half_far_up = up * half_far_size.y;
		vec3d const half_far_right = right * half_far_size.x;
		this->far.tl = far_center + half_far_up - half_far_right;
		this->far.tr = far_center + half_far_up + half_far_right;
		this->far.bl = far_center - half_far_up - half_far_right;
		this->far.br = far_center - half_far_up + half_far_right;


		// XXX normals point to outside of the frustum
		this->_plane(PlanePosition::top, {near.tl, far.tr, far.tl});
		this->_plane(PlanePosition::bottom, {near.br, far.bl, far.br});
		this->_plane(PlanePosition::left, {near.bl, far.tl, far.bl});
		this->_plane(PlanePosition::right, {near.br, far.br, far.tr});
		this->_plane(PlanePosition::near, {near.br, near.tr, near.bl});
		this->_plane(PlanePosition::far, {far.bl, far.tl, far.tr});
	}

	template<typename T>
	mesh::Mesh
	Frustum<T>::mesh() const
	{
		using mesh::Mesh;
		Mesh mesh{Mesh::Kind::vertex, Mesh::Mode::quads};
		vec3d quads[] = {
			near.tr, near.tl, near.bl, near.br, // near
			far.bl, far.br, far.tr, far.tl,     // far
			near.bl, near.tl, far.tl, far.bl,   // left
			near.tr, near.br, far.br, far.tr,   // right
			near.tl, near.tr, far.tr, far.tl,   // top
			near.br, near.bl, far.bl, far.br,   // bottom
		};
		for (vec3d& p: quads)
			mesh.append(vec3f{p.x, p.y, p.z});
		return mesh;
	}

	template<typename T>
	std::unique_ptr<renderer::Drawable>
	Frustum<T>::view(renderer::Renderer& renderer) const
	{
		return this->mesh().view(renderer);
	}

	template struct Frustum<float>;
	template struct Frustum<double>;
	template struct Frustum<int32_t>;
	//template struct Frustum<uint32_t>;
	template struct Frustum<int64_t>;
	//template struct Frustum<uint64_t>;
}}}
