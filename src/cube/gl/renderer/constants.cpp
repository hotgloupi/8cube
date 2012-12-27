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

	std::ostream& operator <<(std::ostream& out, ContentType const type)
	{
		switch (type)
		{
#define _CASE(name)                                                           \
		case ContentType::name:                                               \
			out << "ContentType::" #name;                                     \
			break                                                             \
/**/
		_CASE(int8);
		_CASE(uint8);
		_CASE(int16);
		_CASE(uint16);
		_CASE(int32);
		_CASE(uint32);
		_CASE(float32);
#undef _CASE
		default:
			out << "Unknown ContentType";
		}
		return out;
	}


	std::ostream& operator <<(std::ostream& out, ContentHint const hint)
	{
		switch (hint)
		{
#define _CASE(name)                                                           \
		case ContentHint::name:                                               \
			out << "ContentHint::" #name;                                     \
			break                                                             \
/**/
		_CASE(stream_content);
		_CASE(static_content);
		_CASE(dynamic_content);
#undef _CASE
		default:
			out << "Unknown ContentHint";
		}
		return out;
	}

	std::ostream& operator <<(std::ostream& out, ContentKind const kind)
	{
		switch (kind)
		{
#define _CASE(name)                                                           \
		case ContentKind::name:                                               \
			out << "ContentKind::" #name;                                     \
			break                                                             \
/**/
		_CASE(vertex);
		_CASE(index);
		_CASE(color);
		_CASE(normal);
		_CASE(tex_coord0);
		_CASE(tex_coord1);
		_CASE(tex_coord2);
#undef _CASE
		default:
			out << "Unknown ContentKind";
		}
		return out;
	}

}}}
