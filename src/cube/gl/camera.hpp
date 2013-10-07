#ifndef  CUBE_GL_CAMERA_HPP
# define CUBE_GL_CAMERA_HPP

# include "vector.hpp"

# include <cube/api.hpp>
# include <cube/units/angle.hpp>

namespace cube { namespace gl { namespace camera {

	struct CUBE_API Camera
	{
	public:
		typedef vector::Vector3f vec3;

	private:
		vec3            _position;
		vec3            _front;
		vec3            _up;
		units::Angle    _hrot;
		units::Angle    _vrot;

	public:
		Camera() noexcept
		{ this->rotate(0.0_rad, 0.0_rad); }

		inline
		vec3 const& position() const noexcept
		{ return _position; }

		inline
		vec3 const& front() const noexcept
		{ return _front; }

		inline
		vec3 const& up() const noexcept
		{ return _up; }

		inline
		vec3 right() const noexcept
		{ return vector::normalize(vector::cross(this->_front, this->_up)); }

		Camera& rotate(units::Angle const horizontal_angle,
		               units::Angle const vertical_angle) noexcept;

		inline
		Camera& move(vec3 const& v) noexcept
		{
			_position += v;
			return *this;
		}
	};

}}}

#endif

