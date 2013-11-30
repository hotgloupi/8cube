#include "constants.hpp"
#include "Exception.hpp"

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

	std::ostream& operator <<(std::ostream& out, LightKind const kind)
	{
		switch (kind)
		{
#define _CASE(name)                                                           \
		case LightKind::name:                                                 \
			out << "LightKind::" #name;                                       \
			break                                                             \
/**/
		_CASE(spot);
		_CASE(point);
		_CASE(directional);
		_CASE(custom);
#undef _CASE
		default:
			out << "Unknown LightKind value";
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

	Mask pixel_mask(PixelFormat const fmt)
	{
		Mask res = {0, 0, 0, 0};
		switch (fmt)
		{
		case PixelFormat::depth_component:
		case PixelFormat::depth_stencil:
		case PixelFormat::red:
		case PixelFormat::r8:
		case PixelFormat::r8_snorm:
		case PixelFormat::r8i:
		case PixelFormat::r8ui:
			res.red = 0xff000000;
			break;

		case PixelFormat::r16:
		case PixelFormat::r16_snorm:
		case PixelFormat::r16i:
		case PixelFormat::r16ui:
		case PixelFormat::r16f:
			res.red = 0xffff0000;
			break;

		case PixelFormat::r32f:
		case PixelFormat::r32i:
		case PixelFormat::r32ui:
			res.red = 0xffffffff;
			break;

		case PixelFormat::rg:
		case PixelFormat::rg8i:
		case PixelFormat::rg8ui:
		case PixelFormat::rg8:
		case PixelFormat::rg8_snorm:
			res.red = 0xff000000;
			res.green = 0x00ff0000;
			break;

		case PixelFormat::rg16:
		case PixelFormat::rg16_snorm:
		case PixelFormat::rg16i:
		case PixelFormat::rg16ui:
		case PixelFormat::rg16f:
			res.red = 0xffff0000;
			res.green = 0x0000ffff;
			break;

		case PixelFormat::rg32i:
		case PixelFormat::rg32ui:
		case PixelFormat::rg32f:
		case PixelFormat::rgb16i:
		case PixelFormat::rgb16ui:
		case PixelFormat::rgb16f:
		case PixelFormat::rgb32i:
		case PixelFormat::rgb32ui:
		case PixelFormat::rgba16i:
		case PixelFormat::rgba16ui:
		case PixelFormat::rgba32i:
		case PixelFormat::rgba32ui:
			throw Exception{"Unsupported pixel format size"};

		case PixelFormat::rgb:
		case PixelFormat::rgb8:
		case PixelFormat::rgb8_snorm:
		case PixelFormat::rgb8i:
		case PixelFormat::rgb8ui:
			res.red =   0xff000000;
			res.green = 0x00ff0000;
			res.blue =  0x0000ff00;
			break;

		case PixelFormat::rgba:
		case PixelFormat::rgba8:
		case PixelFormat::rgba8_snorm:
		case PixelFormat::rgba8i:
		case PixelFormat::rgba8ui:
			res.red =   0xff000000;
			res.green = 0x00ff0000;
			res.blue =  0x0000ff00;
			res.alpha = 0x000000ff;
			break;

		case PixelFormat::r3_g3_b2:
		case PixelFormat::rgb4:
		case PixelFormat::rgb5:
		case PixelFormat::rgb10:
		case PixelFormat::rgb12:
		case PixelFormat::rgb16_snorm:
		case PixelFormat::rgba2:
		case PixelFormat::rgba4:
		case PixelFormat::rgb5_a1:
		case PixelFormat::rgb10_a2:
		case PixelFormat::rgb10_a2ui:
		case PixelFormat::rgba12:
		case PixelFormat::rgba16:
		case PixelFormat::srgb8:
		case PixelFormat::srgb8_alpha8:
		case PixelFormat::rgba16f:
		case PixelFormat::rgb32f:
		case PixelFormat::rgba32f:
		case PixelFormat::r11f_g11f_b10f:
		case PixelFormat::rgb9_e5:
		case PixelFormat::compressed_red:
		case PixelFormat::compressed_rg:
		case PixelFormat::compressed_rgb:
		case PixelFormat::compressed_rgba:
		case PixelFormat::compressed_srgb:
		case PixelFormat::compressed_srgb_alpha:
			throw Exception{"Unsupported pixel format"};
		default:
			throw Exception{"Unknown format"};
		}
		return res;
	}

	int pixel_depth(PixelFormat const fmt)
	{
		switch (fmt)
		{
		case PixelFormat::depth_component:
		case PixelFormat::depth_stencil:
		case PixelFormat::red:
		case PixelFormat::r8:
		case PixelFormat::r8_snorm:
		case PixelFormat::r8i:
		case PixelFormat::r8ui:
			return 8;

		case PixelFormat::r16:
		case PixelFormat::r16_snorm:
		case PixelFormat::r16i:
		case PixelFormat::r16ui:
		case PixelFormat::r16f:
			return 16;

		case PixelFormat::r32f:
		case PixelFormat::r32i:
		case PixelFormat::r32ui:
			return 32;
			break;

		case PixelFormat::rg:
		case PixelFormat::rg8i:
		case PixelFormat::rg8ui:
		case PixelFormat::rg8:
		case PixelFormat::rg8_snorm:
			return 16;

		case PixelFormat::rg16:
		case PixelFormat::rg16_snorm:
		case PixelFormat::rg16i:
		case PixelFormat::rg16ui:
		case PixelFormat::rg16f:
			return 32;

		case PixelFormat::rgb:
		case PixelFormat::rgb8:
		case PixelFormat::rgb8_snorm:
		case PixelFormat::rgb8i:
		case PixelFormat::rgb8ui:
			return 24;

		case PixelFormat::rgba:
		case PixelFormat::rgba8:
		case PixelFormat::rgba8_snorm:
		case PixelFormat::rgba8i:
		case PixelFormat::rgba8ui:
			return 32;

		case PixelFormat::rg32i:
		case PixelFormat::rg32ui:
		case PixelFormat::rg32f:
		case PixelFormat::rgb16i:
		case PixelFormat::rgb16ui:
		case PixelFormat::rgb16f:
		case PixelFormat::rgb32i:
		case PixelFormat::rgb32ui:
		case PixelFormat::rgba16i:
		case PixelFormat::rgba16ui:
		case PixelFormat::rgba32i:
		case PixelFormat::rgba32ui:
			throw Exception{"Unsupported pixel format size"};

		case PixelFormat::r3_g3_b2:
		case PixelFormat::rgb4:
		case PixelFormat::rgb5:
		case PixelFormat::rgb10:
		case PixelFormat::rgb12:
		case PixelFormat::rgb16_snorm:
		case PixelFormat::rgba2:
		case PixelFormat::rgba4:
		case PixelFormat::rgb5_a1:
		case PixelFormat::rgb10_a2:
		case PixelFormat::rgb10_a2ui:
		case PixelFormat::rgba12:
		case PixelFormat::rgba16:
		case PixelFormat::srgb8:
		case PixelFormat::srgb8_alpha8:
		case PixelFormat::rgba16f:
		case PixelFormat::rgb32f:
		case PixelFormat::rgba32f:
		case PixelFormat::r11f_g11f_b10f:
		case PixelFormat::rgb9_e5:
		case PixelFormat::compressed_red:
		case PixelFormat::compressed_rg:
		case PixelFormat::compressed_rgb:
		case PixelFormat::compressed_rgba:
		case PixelFormat::compressed_srgb:
		case PixelFormat::compressed_srgb_alpha:
			throw Exception{"Unsupported pixel format"};
		default:
			throw Exception{"Unknown format"};
		}
	}

}}}
