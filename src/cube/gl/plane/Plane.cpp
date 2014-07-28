#include "Plane.hpp"

#include <cube/gl/exception.hpp>

#include <iostream>

namespace cube { namespace gl { namespace plane {

	template<typename T>
	Plane<T>::Plane(vec3 const& p0, vec3 const& p1, vec3 const& p2) ETC_NOEXCEPT
		: Plane{p0, vector::cross(p1 - p0, p2 - p0)}
	{}

	template<typename T>
	Plane<T>::Plane(vec3 const& point,
	                vec3 const& normal) ETC_NOEXCEPT
	{
		auto n = vector::normalize(normal);
		_coef = vec4(n, -vector::dot(n, point));
	}

	template struct Plane<float>;
	template struct Plane<double>;
/*	template struct Plane<int32_t>;
	template struct Plane<uint32_t>;
	template struct Plane<int64_t>;
	template struct Plane<uint64_t>;
*/
	template<typename T>
	std::ostream& operator <<(std::ostream& out, Plane<T> const& plane)
	{
		out << "Plane";
		if (std::is_same<T, float>::value)
			out << "f";
		else if (std::is_same<T, double>::value)
			out << "d";
		else if (std::is_same<T, int32_t>::value)
			out << "i";
		else if (std::is_same<T, uint32_t>::value)
			out << "u";
		else if (std::is_same<T, int64_t>::value)
			out << "il";
		else if (std::is_same<T, uint64_t>::value)
			out << "ul";
		else
			out << "?";
		out << "("
		    << plane.coef()[0] << ", "
		    << plane.coef()[1] << ", "
		    << plane.coef()[2] << ", "
		    << plane.coef()[3] << ")";
		return out;
	}

	template
	CUBE_API std::ostream& operator <<<float>(std::ostream& out, Plane<float> const& plane);
	template
	CUBE_API std::ostream& operator <<<double>(std::ostream& out, Plane<double> const& plane);
	/*
	template
	std::ostream& operator <<<int32_t>(std::ostream& out, Plane<int32_t> const& plane);
	template
	std::ostream& operator <<<uint32_t>(std::ostream& out, Plane<uint32_t> const& plane);
	template
	std::ostream& operator <<<int64_t>(std::ostream& out, Plane<int64_t> const& plane);
	template
	std::ostream& operator <<<uint64_t>(std::ostream& out, Plane<uint64_t> const& plane);
*/
}}}
