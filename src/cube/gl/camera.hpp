#ifndef  CUBE_GL_CAMERA_HPP
# define CUBE_GL_CAMERA_HPP

# include "vector.hpp"

# include <cube/units/angle.hpp>

//XXX
#include <boost/units/io.hpp>
#include <iostream>

namespace cube { namespace gl { namespace camera {

	struct Camera
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
		Camera()
		{ this->rotate(0.0_rad, 0.0_rad); }

		inline
		vec3 const& position() const
		{ return _position; }

		inline
		vec3 const& front() const
		{ return _front; }

		inline
		vec3 const& up() const
		{ return _up; }

		inline
		vec3 right() const
		{ return vector::cross(this->_front, this->_up); }

		Camera& rotate(units::Angle const horizontal_angle,
		               units::Angle const vertical_angle);

		inline
		Camera& move(vec3 const& v)
		{
			_position += v;
			return *this;
		}
	};

}}}

#endif

