#include "constants.hpp"

#include <ostream>

namespace cube { namespace gl { namespace renderer {

	std::ostream& operator <<(std::ostream& out, Mode mode)
	{
		switch (mode)
		{
# define _CASE(name)                                                          \
		case Mode::name:                                                      \
			out << "Mode::" #name;                                            \
			break                                                             \
/**/
			_CASE(none);
			_CASE(_2d);
			_CASE(_3d);
#undef _CASE
		default:
			out << "Unknown Mode";
		}
		return out;
	}

	std::ostream& operator <<(std::ostream& out, MatrixKind kind)
	{
		switch (kind)
		{
#define _CASE(name)                                                           \
		case MatrixKind::name:                                                \
			out << "MatrixKind::" #name;                                      \
			break                                                             \
/**/
		_CASE(model);
		_CASE(view);
		_CASE(projection);
		_CASE(mvp);
#undef _CASE
		default:
			out << "Unknown MatrixKind value";
		}
		return out;
	}

	std::ostream& operator <<(std::ostream& out, ShaderType const type)
	{
		switch (type)
		{
#define _CASE(name)                                                           \
		case ShaderType::name:                                                \
			out << "ShaderType::" #name;                                      \
			break                                                             \
/**/
		_CASE(vertex);
		_CASE(fragment);
#undef _CASE
		default:
			out << "Unknown ShaderType";
		}
		return out;
	}

}}}
