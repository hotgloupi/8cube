#ifndef  CUBE_UNITS_ANGLE_HPP
# define CUBE_UNITS_ANGLE_HPP

# include <boost/units/physical_dimensions/plane_angle.hpp>
# include <boost/units/quantity.hpp>
# include <boost/units/systems/si/plane_angle.hpp>
# include <boost/units/systems/angle/degrees.hpp>

namespace cube { namespace units {

	namespace bu = boost::units;

	typedef float angle_internal_type;

# define _CUBE_UNITS_MAKE_QUANTITY(__name, __base_unit, __storage)            \
	struct __name                                                             \
		: bu::quantity<__base_unit, __storage>                                \
	{                                                                         \
		template<typename... Args>                                            \
		__name(Args&&... args)                                                \
			: bu::quantity<__base_unit, __storage>(                           \
				std::forward<Args>(args)...                                   \
			)                                                                 \
		{}                                                                    \
	}                                                                         \
/**/

	_CUBE_UNITS_MAKE_QUANTITY(Angle, bu::si::plane_angle, angle_internal_type);

	BOOST_UNITS_STATIC_CONSTANT(radian, bu::si::plane_angle);
	_CUBE_UNITS_MAKE_QUANTITY(Radian, bu::si::plane_angle, angle_internal_type);

	BOOST_UNITS_STATIC_CONSTANT(degree, bu::degree::plane_angle);

	_CUBE_UNITS_MAKE_QUANTITY(Degree, bu::degree::plane_angle, angle_internal_type);

	namespace details {

		template<typename T>
		struct suffix_operator_param_type;

		template<>
		struct suffix_operator_param_type<double>
		{ typedef long double type; };

		template<>
		struct suffix_operator_param_type<float>
		{ typedef long double type; };

	} // !details

}} // !cube::units

# define _CUBE_UNITS_ANGLE_CONVERTERS(__ret_type,                             \
                                      __func_name,                            \
                                      __from_type,                            \
                                      __unit,                                 \
                                      __to_type)                              \
namespace cube { namespace units {                                            \
	inline                                                                    \
	__ret_type __func_name(__from_type value)                                 \
	{ return __ret_type(value * __unit); }                                    \
	template<typename T>                                                      \
	inline                                                                    \
	__from_type __func_name ## _value(T value)                                \
	{ return static_cast<__to_type>(value).value(); }                         \
}}                                                                            \
inline                                                                        \
auto operator"" _##__func_name(                                               \
		cube::units::details::suffix_operator_param_type<__from_type>::type v \
	)                                                                         \
	-> decltype(cube::units::__func_name(v))                                  \
{ return cube::units::__func_name(v); }                                       \
/**/

// Angle rad(double)
// Angle operator"" _rad(double)
// double rad_value(T)
_CUBE_UNITS_ANGLE_CONVERTERS(Angle, rad, float, radian, Radian);

// Angle deg(double)
// Angle operator"" _deg(double)
// double deg_value(T)
_CUBE_UNITS_ANGLE_CONVERTERS(Angle, deg, float, degree, Degree);

# undef _CUBE_UNITS_ANGLE_CONVERTERS

#endif
