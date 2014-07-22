#include "camera.hpp"
#include "matrix.hpp"

#include <etc/assert.hpp>
#include <etc/print.hpp>

#include <glm/gtx/quaternion.hpp>

namespace cube { namespace gl { namespace camera {

	struct Camera::Impl
	{
		vec3 position;
		glm::quat yaw; // up vector
		glm::quat pitch; // right vector
		glm::quat roll; // front vector
	};

	Camera::Camera()
		: _this{new Impl}
	{}

	Camera::~Camera()
	{}

	Camera::vec3 const& Camera::position() const ETC_NOEXCEPT
	{ return _this->position; }

	Camera& Camera::position(vec3 const& position) ETC_NOEXCEPT
	{ _this->position = position; return *this; }

	Camera& Camera::look_at(vec3 const& position) ETC_NOEXCEPT
	{
		ETC_ASSERT_NEQ(_this->position, position);
		vec3 target = glm::normalize(position - _this->position);

		return *this;
	}

	Camera::vec3 Camera::front() const ETC_NOEXCEPT
	{ return glm::axis(_this->roll); }

	Camera::vec3 Camera::up() const ETC_NOEXCEPT
	{ return glm::axis(_this->yaw); }

	Camera::vec3 Camera::right() const ETC_NOEXCEPT
	{ return glm::axis(_this->pitch); }

	Camera& Camera::move(vec3 const& v) ETC_NOEXCEPT
	{ _this->position += v; return *this; }

	//Camera&
	//Camera::rotate(units::Angle const horizontal_angle,
	//               units::Angle const vertical_angle) ETC_NOEXCEPT
	//{
	//	_hrot += horizontal_angle;
	//	while (_hrot < units::rad(0))
	//		_hrot += units::rad(3.14159265f*2);
	//	while (_hrot > units::rad(3.14159265f*2))
	//		_hrot -= units::rad(3.14159265f*2);

	//	_vrot += vertical_angle;
	//	if (_vrot > units::rad(3.14159f / 2))
	//		_vrot = units::rad(3.14159f / 2 - 0.01f);
	//	if (_vrot < units::rad(-3.14159f / 2))
	//		_vrot = units::rad(-3.14159f / 2 + 0.01f);

	//	_front = vector::rotate_x(vec3{0, 0, -1}, units::deg_value(_vrot));
	//	_front = vector::rotate_y(_front, units::deg_value(_hrot));
	//	_up = vector::rotate_x(vec3{0, 1, 0}, units::deg_value(_vrot));
	//	_up = vector::rotate_y(_up, units::deg_value(_hrot));

	//	double dot = vector::dot(_up, _front);
	//	assert(dot < 1e-7);
	//	return *this;
	//}

	//void Camera::look_at(vec3 const& position) ETC_NOEXCEPT
	//{
	//	_front = vector::normalize(position - _position);
	//	vec3 right = this->right();
	//	_up = vector::normalize(vector::cross(right, _front));
	//}

}}}
