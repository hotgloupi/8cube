#include "sphere.hpp"

#include <iostream>

namespace cube { namespace gl { namespace sphere {

	template<typename T>
	std::ostream& operator <<(std::ostream& out, Sphere<T> const& sphere)
	{
		out << "Sphere";
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
		out << "(("
		    << sphere.center.x << ", "
		    << sphere.center.y << ", "
		    << sphere.center.z << "), "
		    << sphere.radius << ")";
		return out;
	}

	template
	std::ostream&
	operator <<<float>(std::ostream& out, Sphere<float> const& sphere);

	template
	std::ostream&
	operator <<<double>(std::ostream& out, Sphere<double> const& sphere);

	template
	std::ostream&
	operator <<<int32_t>(std::ostream& out, Sphere<int32_t> const& sphere);

	template
	std::ostream&
	operator <<<uint32_t>(std::ostream& out, Sphere<uint32_t> const& sphere);

	template
	std::ostream&
	operator <<<int64_t>(std::ostream& out, Sphere<int64_t> const& sphere);

	template
	std::ostream&
	operator <<<uint64_t>(std::ostream& out, Sphere<uint64_t> const& sphere);

}}}
