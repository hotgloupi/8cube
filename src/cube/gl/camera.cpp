#include "camera.hpp"
#include "matrix.hpp"

#include <etc/assert.hpp>
#include <etc/print.hpp>
#include <etc/test.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace cube { namespace gl { namespace camera {

#define CAMERA_FRONT Camera::vec3(0.0f, 0.0f, -1.0f)
#define CAMERA_RIGHT Camera::vec3(1.0f, 0.0f, 0.0f)
#define CAMERA_UP    Camera::vec3(0.0f, 1.0f, 0.0f)

	struct Camera::Impl
	{
		vec3 position;
		glm::quat orientation;

		Impl()
		{}

		Impl(Impl const& other)
			: position(other.position)
			, orientation(other.orientation)
		{}
	};

	Camera::Camera()
		: _this{new Impl}
	{}

	Camera::Camera(Camera const& other)
		: _this{new Impl(*other._this)}
	{}

	Camera::Camera(Camera&& other)
		: _this{std::move(other._this)}
	{}

	Camera::~Camera()
	{}

	units::Angle Camera::pitch() const ETC_NOEXCEPT
	{ return units::deg(glm::pitch(_this->orientation)); }

	units::Angle Camera::yaw() const ETC_NOEXCEPT
	{ return units::deg(glm::yaw(_this->orientation)); }

	units::Angle Camera::roll() const ETC_NOEXCEPT
	{ return units::deg(glm::roll(_this->orientation)); }

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
	{ return CAMERA_FRONT * _this->orientation; }

	Camera::vec3 Camera::up() const ETC_NOEXCEPT
	{ return CAMERA_UP * _this->orientation; }

	Camera::vec3 Camera::right() const ETC_NOEXCEPT
	{ return CAMERA_RIGHT * _this->orientation; }

	Camera& Camera::move(vec3 const& v) ETC_NOEXCEPT
	{ _this->position += v; return *this; }

	Camera& Camera::rotate(units::Angle const& angle,
	                       vec3 const& axis) ETC_NOEXCEPT
	{
		_this->orientation = glm::rotate(
			_this->orientation,
			units::rad_value(angle),
			axis
		);
		return *this;
	}

	Camera& Camera::rotate_x(units::Angle const& angle) ETC_NOEXCEPT
	{ return this->rotate(angle, CAMERA_RIGHT); }

	Camera& Camera::rotate_y(units::Angle const& angle) ETC_NOEXCEPT
	{ return this->rotate(angle, CAMERA_UP); }

	Camera& Camera::rotate_z(units::Angle const& angle) ETC_NOEXCEPT
	{ return this->rotate(angle, CAMERA_FRONT); }

	//void Camera::look_at(vec3 const& position) ETC_NOEXCEPT
	//{
	//	_front = vector::normalize(position - _position);
	//	vec3 right = this->right();
	//	_up = vector::normalize(vector::cross(right, _front));
	//}

	namespace {

		typedef Camera::vec3 vec3;

		ETC_TEST_CASE(init)
		{
			auto c = Camera();
			ETC_TEST_EQ(units::deg_value(c.yaw()), 0.0f);
			ETC_TEST_EQ(units::deg_value(c.pitch()), 0.0f);
			ETC_TEST_EQ(units::deg_value(c.roll()), 0.0f);
			ETC_TEST_EQ(c.position(), vec3(0, 0, 0));
			ETC_TEST_EQ(c.front(), CAMERA_FRONT);
			ETC_TEST_EQ(c.up(), CAMERA_UP);
			ETC_TEST_EQ(c.right(), CAMERA_RIGHT);
		}

		ETC_TEST_CASE(move)
		{
			Camera c;
			c.move(vec3(1,3,5));
			ETC_TEST_EQ(c.position(), vec3(1,3,5));
			c.move(vec3(1,3,5));
			ETC_TEST_EQ(c.position(), vec3(2,6,10));
		}


		ETC_TEST_CASE(rotate)
		{
			Camera c;
			c.rotate_y(units::deg(-90));
			ETC_TEST_EQ(c.front(), -CAMERA_RIGHT);
			ETC_TEST_EQ(c.right(), CAMERA_FRONT);
		}

	}

}}}
