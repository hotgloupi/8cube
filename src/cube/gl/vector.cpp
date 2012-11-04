#include "vector.hpp"

#include <etc/types.hpp>

#include <ostream>

namespace glm { namespace detail {

# define _CUBE_GL_VECTOR_DEF(__arity, __value_type, __suffix)                 \
	std::ostream& operator <<(std::ostream& out,                              \
	                          tvec ## __arity<__value_type> const& value)     \
	{                                                                         \
		out << "Vector" #__arity  #__suffix "(";                              \
		bool first = true;                                                    \
		for (etc::size_type i = 0; i < __arity; ++i)                          \
		{                                                                     \
			if (first)                                                        \
				first = false;                                                \
			else                                                              \
				out << ", ";                                                  \
			out << reinterpret_cast<__value_type const*>(&value)[i];          \
		}                                                                     \
		out << ")";                                                           \
		return out;                                                           \
	}                                                                         \
/**/

	_CUBE_GL_VECTOR_DEF(2, float, f);
	_CUBE_GL_VECTOR_DEF(2, double, d);
	_CUBE_GL_VECTOR_DEF(2, int32_t, i);
	_CUBE_GL_VECTOR_DEF(2, uint32_t, ui);

	_CUBE_GL_VECTOR_DEF(3, float, f);
	_CUBE_GL_VECTOR_DEF(3, double, d);
	_CUBE_GL_VECTOR_DEF(3, int32_t, i);
	_CUBE_GL_VECTOR_DEF(3, uint32_t, ui);

}}

