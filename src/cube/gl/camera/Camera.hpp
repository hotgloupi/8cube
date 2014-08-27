#ifndef  CUBE_GL_CAMERA_CAMERA_HPP
# define CUBE_GL_CAMERA_CAMERA_HPP

# include <cube/api.hpp>
# include <cube/gl/fwd.hpp>
# include <cube/gl/renderer/Bindable.hpp>
# include <cube/gl/vector.hpp>
# include <cube/units/angle.hpp>

# include <etc/compiler.hpp>

namespace cube { namespace gl { namespace camera {

	struct CUBE_API Camera
		: public renderer::Bindable
	{
	public:
		typedef vector::Vector3f vec3;
		typedef frustum::Frustumf frustum_t;

	private:
		struct Impl;
		std::unique_ptr<Impl> _this;

	public:
		Camera();
		Camera(Camera const& other);
		Camera(Camera&& other);
		~Camera();

		/// Rotation around the X axis (left-right).
		units::Angle pitch() const ETC_NOEXCEPT;
		Camera& pitch(units::Angle const angle) ETC_NOEXCEPT;

		/// Rotation around the Y axis (top-down).
		units::Angle yaw() const ETC_NOEXCEPT;
		Camera& yaw(units::Angle const angle) ETC_NOEXCEPT;

		/// Rotation around the -Z axis (front-back).
		units::Angle roll() const ETC_NOEXCEPT;
		Camera& roll(units::Angle const angle) ETC_NOEXCEPT;

		/// Rotate around arbitrary axis.
		Camera& rotate(units::Angle const& angle, vec3 const& axis) ETC_NOEXCEPT;

		/// Position.
		vec3 const& position() const ETC_NOEXCEPT;
		Camera& position(vec3 const& position) ETC_NOEXCEPT;

		/// Front direction.
		vec3 front() const ETC_NOEXCEPT;

		/// Up direction.
		vec3 up() const ETC_NOEXCEPT;

		/// Right direction.
		vec3 right() const ETC_NOEXCEPT;

		/// Translate the camera.
		Camera& move(vec3 const& v) ETC_NOEXCEPT;

		/**
		 * A frustum can be optionally contained in the camera.
		 *
		 * When the a frustum is associated to a camera, it gets automatically
		 * updated when the camera rotates.
		 * When no frustum has been set, the getter raises an exception.
		 */
	public:
		bool has_frustum() const ETC_NOEXCEPT;
		frustum_t frustum() const;
		Camera& init_frustum(units::Angle fov,
		                     float const ratio,
		                     float const ndist,
		                     float const fdist) ETC_NOEXCEPT;

		Camera& look_at(vec3 const& position) ETC_NOEXCEPT;


	private:
		void _bind() override;
		void _unbind() ETC_NOEXCEPT override;
	};

}}}

#endif
