#include "Camera.hpp"

#include <cube/gl/matrix.hpp>
#include <cube/gl/frustum.hpp>

#include <cube/exception.hpp>

#include <etc/assert.hpp>
#include <etc/print.hpp>
#include <etc/test.hpp>
#include <etc/stack_ptr.hpp>

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
		etc::stack_ptr<Camera::frustum_t> frustum;

		Impl()
			: frustum(etc::stack_ptr_no_init)
		{}

		Impl(Impl const& other)
			: position(other.position)
			, orientation(other.orientation)
			, frustum(other.frustum)
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
	{ return units::rad(glm::pitch(_this->orientation)); }

	units::Angle Camera::yaw() const ETC_NOEXCEPT
	{ return units::rad(glm::yaw(_this->orientation)); }

	units::Angle Camera::roll() const ETC_NOEXCEPT
	{ return -units::rad(glm::roll(_this->orientation)); }

	Camera::vec3 const& Camera::position() const ETC_NOEXCEPT
	{ return _this->position; }

	Camera& Camera::position(vec3 const& position) ETC_NOEXCEPT
	{ _this->position = position; return *this; }


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

	Camera& Camera::pitch(units::Angle const angle) ETC_NOEXCEPT
	{ return this->rotate(angle, CAMERA_RIGHT); }

	Camera& Camera::yaw(units::Angle const angle) ETC_NOEXCEPT
	{ return this->rotate(angle, CAMERA_UP); }

	Camera& Camera::roll(units::Angle const angle) ETC_NOEXCEPT
	{ return this->rotate(angle, CAMERA_FRONT); }

	// Camera& Camera::look_at(vec3 const& position) ETC_NOEXCEPT
	// {
	// 	ETC_ASSERT_NEQ(_this->position, position);
	// 	vec3 target = glm::normalize(position - _this->position);

	// 	return *this;
	// }
	bool Camera::has_frustum() const ETC_NOEXCEPT
	{ return _this->frustum; }

	Camera::frustum_t Camera::frustum() const
	{
		if (!_this->frustum)
			throw exception::Exception("No frustum set");
		_this->frustum->update(this->front(), this->up());
		return *_this->frustum;
	}

	Camera& Camera::init_frustum(units::Angle fov,
	                             float const ratio,
	                             float const ndist,
	                             float const fdist) ETC_NOEXCEPT
	{ _this->frustum.reset(fov, ratio, ndist, fdist); return *this; }

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
			c.yaw(units::deg(-90));
			ETC_TEST_LT(std::fabs(glm::length(c.front() - -CAMERA_RIGHT)), 1e-6);
			ETC_TEST_LT(std::fabs(glm::length(c.right() - CAMERA_FRONT)), 1e-6);
		}

	}

}}}
