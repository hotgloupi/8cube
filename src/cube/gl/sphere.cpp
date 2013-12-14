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

#define INSTANCIATE_FOR(__type) \
	template struct CUBE_API Sphere<__type>; \
	template CUBE_API \
	std::ostream& operator <<<__type>(std::ostream& out, \
	                                  Sphere<__type> const& sphere) \
/**/

	INSTANCIATE_FOR(float);
	INSTANCIATE_FOR(double);
	INSTANCIATE_FOR(int32_t);
	INSTANCIATE_FOR(uint32_t);
	INSTANCIATE_FOR(int64_t);
	INSTANCIATE_FOR(uint64_t);
#undef INSTANCIATE_FOR

}}}
