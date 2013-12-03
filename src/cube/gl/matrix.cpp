#include "matrix.hpp"

#include <ostream>

using namespace cube::gl::matrix;

std::ostream&
operator <<(std::ostream& out,
            mat4f const& m)
{
	float const* tab = reinterpret_cast<float const*>(&m);
	out << "mat4f([";
	for (size_t i = 0; i < sizeof(m) / sizeof(mat4f::value_type); ++i)
	{
		if (i) out << ", ";
		if (i % 4 == 0) out << '[';
		out << tab[i];
		if ((i + 5) % 4 == 0) out << ']';
	}
	out << "])";
	return out;
}

