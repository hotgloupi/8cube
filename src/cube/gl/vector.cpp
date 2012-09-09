#include "vector.hpp"

#include <etc/types.hpp>

#include <ostream>

namespace cube { namespace gl { namespace vector {

# define _CUBE_GL_VECTOR_DEF(arity_)                                          \
	std::ostream& operator <<(std::ostream& out,                              \
	                          Vector##arity_<float> const& value)             \
	{                                                                         \
		out << "Vector" #arity_ "f(";                                         \
		bool first = true;                                                    \
		for (etc::size_type i = 0; i < arity_; ++i)                           \
		{                                                                     \
			if (first)                                                        \
				first = false;                                                \
			else                                                              \
				out << ", ";                                                  \
			out << reinterpret_cast<float const*>(&value)[i];                 \
		}                                                                     \
		out << ")";                                                           \
		return out;                                                           \
	}                                                                         \
/**/

	_CUBE_GL_VECTOR_DEF(2);
	_CUBE_GL_VECTOR_DEF(3);

}}}

