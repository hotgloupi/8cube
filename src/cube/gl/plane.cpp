#include "plane.hpp"

#include <iostream>

namespace cube { namespace gl { namespace plane {

	template<typename T>
	Plane<T>::Plane(vec3 const& p0, vec3 const& p1, vec3 const& p2)
		: Plane{p0, vector::cross(p1 - p0, p2 - p0)}
	{}

	template<typename T>
	Plane<T>::Plane(vec3 const& point,
	                vec3 const& normal)
		: _coef{normal.x, normal.y, normal.z, -vector::dot(normal, point)}
	{
		double magnitude = glm::sqrt(
			_coef.x * _coef.x + _coef.y * _coef.y + _coef.z * _coef.z
		);
		if (magnitude == 0)
			magnitude = 1.0;
		for (etc::size_type i = 0; i < 4; ++i)
			_coef[i] /= magnitude;
	}

	template struct Plane<float>;
	template struct Plane<double>;
	template struct Plane<int32_t>;
	template struct Plane<uint32_t>;
	template struct Plane<int64_t>;
	template struct Plane<uint64_t>;

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
	std::ostream& operator <<<float>(std::ostream& out, Plane<float> const& plane);
	template
	std::ostream& operator <<<double>(std::ostream& out, Plane<double> const& plane);
	template
	std::ostream& operator <<<int32_t>(std::ostream& out, Plane<int32_t> const& plane);
	template
	std::ostream& operator <<<uint32_t>(std::ostream& out, Plane<uint32_t> const& plane);
	template
	std::ostream& operator <<<int64_t>(std::ostream& out, Plane<int64_t> const& plane);
	template
	std::ostream& operator <<<uint64_t>(std::ostream& out, Plane<uint64_t> const& plane);

}}}
