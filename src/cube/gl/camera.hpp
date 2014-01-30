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
		vec3            _position;
		vec3            _front;
		vec3            _up;
		units::Angle    _hrot;
		units::Angle    _vrot;

	public:
		Camera() ETC_NOEXCEPT
		{ this->rotate(units::deg(0.0f), units::deg(0.0f)); }

		inline
		vec3 const& position() const ETC_NOEXCEPT
		{ return _position; }

		inline
		vec3 const& front() const ETC_NOEXCEPT
		{ return _front; }

		inline
		vec3 const& up() const ETC_NOEXCEPT
		{ return _up; }

		inline
		vec3 right() const ETC_NOEXCEPT
		{ return vector::normalize(vector::cross(this->_front, this->_up)); }

		Camera& rotate(units::Angle const horizontal_angle,
		               units::Angle const vertical_angle) ETC_NOEXCEPT;

		inline Camera& move(vec3 const& v) ETC_NOEXCEPT
		{
			_position += v;
			return *this;
		}
	};

}}}

#endif
