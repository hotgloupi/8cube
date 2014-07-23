#ifndef  CUBE_UNITS_ANGLE_HPP
# define CUBE_UNITS_ANGLE_HPP

# include <boost/units/physical_dimensions/plane_angle.hpp>
# include <boost/units/quantity.hpp>
# include <boost/units/systems/si/plane_angle.hpp>
# include <boost/units/systems/angle/degrees.hpp>

namespace cube { namespace units {

	namespace bu = boost::units;

	typedef bu::quantity<bu::si::plane_angle, float> Angle;

	BOOST_UNITS_STATIC_CONSTANT(radian_unit, bu::si::plane_angle);
	typedef Angle Radian;

	BOOST_UNITS_STATIC_CONSTANT(degree_unit, bu::degree::plane_angle);
	typedef bu::quantity<bu::degree::plane_angle, float> Degree;

	inline Angle rad(float value)
	{ return Angle(value * radian_unit); }

	template<typename T = float>
	inline T rad_value(Angle a)
	{ return static_cast<T>(Radian(a).value()); }

	inline Angle deg(float value)
	{ return Angle(value * degree_unit); }

	template<typename T = float>
	inline T deg_value(Angle a)
	{ return static_cast<T>(Degree(a).value()); }

}} // !cube::units

#endif
