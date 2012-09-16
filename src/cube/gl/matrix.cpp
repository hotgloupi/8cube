#include "matrix.hpp"

#include <ostream>

using namespace cube::gl::matrix;

std::ostream&
operator <<(std::ostream& out,
            mat44f const& m)
{
	float const* tab = reinterpret_cast<float const*>(&m);
	out << "Matrix([";
	for (size_t i = 0; i < sizeof(m) / sizeof(mat44f::value_type); ++i)
	{
		if (i) out << ", ";
		if (i % 4 == 0) out << '[';
		out << tab[i];
		if ((i + 5) % 4 == 0) out << ']';
	}
	out << "])";
	return out;
}

