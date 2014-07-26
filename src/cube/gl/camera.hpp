#ifndef  CUBE_GL_CAMERA_HPP
# define CUBE_GL_CAMERA_HPP

# include "vector.hpp"

# include <cube/api.hpp>
# include <cube/units/angle.hpp>

# include <etc/compiler.hpp>

namespace cube { namespace gl { namespace camera {

	struct CUBE_API Camera
	{
	public:
		typedef vector::Vector3f vec3;

	private:
		struct Impl;
		std::unique_ptr<Impl> _this;

	public:
		Camera();
		Camera(Camera const& other);
		Camera(Camera&& other);
		~Camera();

		/// Rotation around the X axis (left-right)
		units::Angle pitch() const ETC_NOEXCEPT;
		Camera& pitch(units::Angle const angle) ETC_NOEXCEPT;

		/// Rotation around the Y axis (top-down)
		units::Angle yaw() const ETC_NOEXCEPT;
		Camera& yaw(units::Angle const angle) ETC_NOEXCEPT;

		/// Rotation around the -Z axis (front-back)
		units::Angle roll() const ETC_NOEXCEPT;
		Camera& roll(units::Angle const angle) ETC_NOEXCEPT;

		vec3 const& position() const ETC_NOEXCEPT;
		Camera& position(vec3 const& position) ETC_NOEXCEPT;
		Camera& look_at(vec3 const& position) ETC_NOEXCEPT;
		vec3 front() const ETC_NOEXCEPT;
		vec3 up() const ETC_NOEXCEPT;
		vec3 right() const ETC_NOEXCEPT;
		Camera& move(vec3 const& v) ETC_NOEXCEPT;

		Camera& rotate(units::Angle const& angle, vec3 const& axis) ETC_NOEXCEPT;
	};

}}}

#endif
