#include "camera.hpp"
#include "matrix.hpp"
#include <etc/print.hpp>

namespace cube { namespace gl { namespace camera {

	Camera&
	Camera::rotate(units::Angle const horizontal_angle,
	               units::Angle const vertical_angle) ETC_NOEXCEPT
	{
		_hrot += horizontal_angle;
		while (_hrot < units::rad(0))
			_hrot += units::rad(3.14159265f*2);
		while (_hrot > units::rad(3.14159265f*2))
			_hrot -= units::rad(3.14159265f*2);

		_vrot += vertical_angle;
		if (_vrot > units::rad(3.14159f / 2))
			_vrot = units::rad(3.14159f / 2 - 0.01f);
		if (_vrot < units::rad(-3.14159f / 2))
			_vrot = units::rad(-3.14159f / 2 + 0.01f);

		_front = vector::rotate_x(vec3{0,0,-1}, units::deg_value(_vrot));
		_front = vector::rotate_y(_front, units::deg_value(_hrot));
		_up = vector::rotate_x(vec3{0,1,0}, units::deg_value(_vrot));
		_up = vector::rotate_y(_up, units::deg_value(_hrot));

		double dot = vector::dot(_up, _front);
		assert(dot < 1e-7);
		return *this;
	}

}}}
