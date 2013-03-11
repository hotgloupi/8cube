#include "camera.hpp"

namespace cube { namespace gl { namespace camera {

	Camera&
	Camera::rotate(units::Angle const horizontal_angle,
	               units::Angle const vertical_angle)
	{
		_hrot += horizontal_angle;
		_vrot += vertical_angle;
		if (_vrot > units::rad(3.14159 / 2))
			_vrot = units::rad(3.14159 / 2 - 0.01);
		if (_vrot < units::rad(-3.14159 / 2))
			_vrot = units::rad(-3.14159 / 2 + 0.01);
		_front = vector::normalize(
			vector::rotate(
				vector::rotate(
					vec3{0,0,1},
					units::deg_value(_vrot),
					vec3{1,0,0}
				),
				units::deg_value(_hrot),
				vec3{0,1,0}
			)
		);
		_up = vector::normalize(
			vector::rotate(
				vec3{0,1,0},
				units::deg_value(_vrot),
				vec3{1,0,0}
			)
		);
		return *this;
	}

}}}
