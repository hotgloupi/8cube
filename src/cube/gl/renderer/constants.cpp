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

	std::ostream& operator <<(std::ostream& out, TextureFilter const filter)
	{
		switch (filter)
		{
#define _CASE(name)                                                           \
		case TextureFilter::name:                                             \
			out << "TextureFilter::" #name;                                   \
			break                                                             \
/**/
		_CASE(linear);
		_CASE(nearest);
#undef _CASE
		default:
			out << "Unknown TextureFilter value";
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

	std::ostream& operator <<(std::ostream& out, ContentPacking const value)
	{
		switch (value)
		{
#define _CASE(name)                                                           \
		case ContentPacking::name:                                            \
			out << "ContentPacking::" #name;                                  \
			break                                                             \
/**/
		_CASE(int8);
		_CASE(uint8);
		_CASE(int16);
		_CASE(uint16);
		_CASE(int32);
		_CASE(uint32);
		_CASE(float32);
		_CASE(uint8_3_3_2);
		_CASE(uint8_2_3_3_rev);
		_CASE(uint16_5_6_5);
		_CASE(uint16_5_6_5_rev);
		_CASE(uint16_4_4_4_4);
		_CASE(uint16_4_4_4_4_rev);
		_CASE(uint16_5_5_5_1);
		_CASE(uint16_1_5_5_5_rev);
		_CASE(uint32_8_8_8_8);
		_CASE(uint32_8_8_8_8_rev);
		_CASE(uint32_10_10_10_2);
		_CASE(uint32_2_10_10_10_rev);
#undef _CASE
		default:
			out << "ContentPacking::INVALID";
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
#define CASE(name)                                                            \
		case PixelFormat::name:                                               \
			out << "PixelFormat::" #name;                                     \
			break                                                             \
/**/
		CASE(depth_component);
		CASE(depth_stencil);
		CASE(red);
		CASE(rg);
		CASE(rgb);
		CASE(rgba);
		CASE(r8);
		CASE(r8_snorm);
		CASE(r16);
		CASE(r16_snorm);
		CASE(rg8);
		CASE(rg8_snorm);
		CASE(rg16);
		CASE(rg16_snorm);
		CASE(r3_g3_b2);
		CASE(rgb4);
		CASE(rgb5);
		CASE(bgr5);
		CASE(rgb8);
		CASE(rgb8_snorm);
		CASE(bgr8);
		CASE(rgb10);
		CASE(rgb12);
		CASE(rgb16_snorm);
		CASE(rgba2);
		CASE(argb4);
		CASE(rgba4);
		CASE(abgr4);
		CASE(bgra4);
		CASE(a1_rgb5);
		CASE(a1_bgr5);
		CASE(bgr5_a1);
		CASE(rgb5_a1);
		CASE(argb8);
		CASE(rgba8);
		CASE(abgr8);
		CASE(bgra8);
		CASE(rgba8_snorm);
		CASE(a2_rgb10);
		CASE(rgb10_a2);
		CASE(rgb10_a2ui);
		CASE(rgba12);
		CASE(rgba16);
		CASE(srgb8);
		CASE(srgb8_alpha8);
		CASE(r16f);
		CASE(rg16f);
		CASE(rgb16f);
		CASE(rgba16f);
		CASE(r32f);
		CASE(rg32f);
		CASE(rgb32f);
		CASE(rgba32f);
		CASE(r11f_g11f_b10f);
		CASE(rgb9_e5);
		CASE(r8i);
		CASE(r8ui);
		CASE(r16i);
		CASE(r16ui);
		CASE(r32i);
		CASE(r32ui);
		CASE(rg8i);
		CASE(rg8ui);
		CASE(rg16i);
		CASE(rg16ui);
		CASE(rg32i);
		CASE(rg32ui);
		CASE(rgb8i);
		CASE(rgb8ui);
		CASE(rgb16i);
		CASE(rgb16ui);
		CASE(rgb32i);
		CASE(rgb32ui);
		CASE(rgba8i);
		CASE(rgba8ui);
		CASE(rgba16i);
		CASE(rgba16ui);
		CASE(rgba32i);
		CASE(rgba32ui);
		CASE(compressed_red);
		CASE(compressed_rg);
		CASE(compressed_rgb);
		CASE(compressed_rgba);
		CASE(compressed_srgb);
		CASE(compressed_srgb_alpha);
#undef CASE
		default:
			out << "PixelFormat::INVALID";
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
		case PixelFormat::bgr8:
			res.blue =  0xff000000;
			res.green = 0x00ff0000;
			res.red =   0x0000ff00;
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
		case PixelFormat::argb8:
			res.alpha = 0xff000000;
			res.red =   0x00ff0000;
			res.green = 0x0000ff00;
			res.blue =  0x000000ff;
			break;
		case PixelFormat::abgr8:
			res.alpha = 0xff000000;
			res.blue =  0x00ff0000;
			res.green = 0x0000ff00;
			res.red =   0x000000ff;
			break;
		case PixelFormat::bgra8:
			res.blue =  0xff000000;
			res.green = 0x00ff0000;
			res.red =   0x0000ff00;
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
