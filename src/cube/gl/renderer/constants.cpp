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

	std::ostream& operator<<(std::ostream& out, DrawMode const mode)
	{
		switch (mode)
		{
#define _CASE(name)                                                           \
		case DrawMode::name:                                                  \
			out << "DrawMode::" #name;                                        \
			break                                                             \
/**/
		_CASE(points);
		_CASE(lines);
		_CASE(line_strip);
		_CASE(line_loop);
		_CASE(triangles);
		_CASE(triangle_strip);
		_CASE(triangle_fan);
		_CASE(quads);
		_CASE(quad_strip);
		_CASE(polygon);
#undef _CASE
		default:
			out << "Unknown DrawMode";
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

	std::ostream& operator <<(std::ostream& out, PixelFormat const format)
	{
		switch (format)
		{
#define _CASE(name)                                                           \
		case PixelFormat::name:                                               \
			out << "PixelFormat::" #name;                                     \
			break                                                             \
/**/
		_CASE(depth_component);
		_CASE(depth_stencil);
		_CASE(red);
		_CASE(rg);
		_CASE(rgb);
		_CASE(rgba);
		_CASE(r8);
		_CASE(r8_snorm);
		_CASE(r16);
		_CASE(r16_snorm);
		_CASE(rg8);
		_CASE(rg8_snorm);
		_CASE(rg16);
		_CASE(rg16_snorm);
		_CASE(r3_g3_b2);
		_CASE(rgb4);
		_CASE(rgb5);
		_CASE(rgb8);
		_CASE(rgb8_snorm);
		_CASE(rgb10);
		_CASE(rgb12);
		_CASE(rgb16_snorm);
		_CASE(rgba2);
		_CASE(rgba4);
		_CASE(rgb5_a1);
		_CASE(rgba8);
		_CASE(rgba8_snorm);
		_CASE(rgb10_a2);
		_CASE(rgb10_a2ui);
		_CASE(rgba12);
		_CASE(rgba16);
		_CASE(srgb8);
		_CASE(srgb8_alpha8);
		_CASE(r16f);
		_CASE(rg16f);
		_CASE(rgb16f);
		_CASE(rgba16f);
		_CASE(r32f);
		_CASE(rg32f);
		_CASE(rgb32f);
		_CASE(rgba32f);
		_CASE(r11f_g11f_b10f);
		_CASE(rgb9_e5);
		_CASE(r8i);
		_CASE(r8ui);
		_CASE(r16i);
		_CASE(r16ui);
		_CASE(r32i);
		_CASE(r32ui);
		_CASE(rg8i);
		_CASE(rg8ui);
		_CASE(rg16i);
		_CASE(rg16ui);
		_CASE(rg32i);
		_CASE(rg32ui);
		_CASE(rgb8i);
		_CASE(rgb8ui);
		_CASE(rgb16i);
		_CASE(rgb16ui);
		_CASE(rgb32i);
		_CASE(rgb32ui);
		_CASE(rgba8i);
		_CASE(rgba8ui);
		_CASE(rgba16i);
		_CASE(rgba16ui);
		_CASE(rgba32i);
		_CASE(rgba32ui);
		_CASE(compressed_red);
		_CASE(compressed_rg);
		_CASE(compressed_rgb);
		_CASE(compressed_rgba);
		_CASE(compressed_srgb);
		_CASE(compressed_srgb_alpha);
		default:
			out << "Unknown PixelFormat";
		}
		return out;
	}
}}}
