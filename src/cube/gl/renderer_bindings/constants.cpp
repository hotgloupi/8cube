#include "exports.hpp"

#include <wrappers/boost/python.hpp>

#include "../renderer/constants.hpp"

namespace cube { namespace gl { namespace renderer_bindings {

	namespace py = boost::python;
	using namespace ::cube::gl::renderer;

	void export_constants()
	{
		py::enum_<Name>(
				"Name", "Availables renderer names"
			)
			.value("OpenGL", Name::OpenGL)
			.value("DirectX", Name::DirectX)
		;

		py::enum_<Mode>(
				"RendererMode",
				"Renderer mode affects default projection matrices"
			)
			.value("mode_2d", Mode::_2d)
			.value("mode_3d", Mode::_3d)
			.export_values()
			.value("none", Mode::none)
			.value("_2d", Mode::_2d)
			.value("_3d", Mode::_3d)
		;

		py::enum_<ContentKind>("ContentKind")
			.value("vertex", ContentKind::vertex)
			.value("index", ContentKind::index)
			.value("color", ContentKind::color)
			.value("normal", ContentKind::normal)
			.value("tex_coord0", ContentKind::tex_coord0)
			.value("tex_coord1", ContentKind::tex_coord1)
			.value("tex_coord2", ContentKind::tex_coord2)
		;

		py::enum_<ShaderType>("ShaderType")
			.value("fragment", ShaderType::fragment)
			.value("vertex", ShaderType::vertex)
		;

		py::enum_<ShaderParameterType>("ShaderParameterType")
			.value("float", ShaderParameterType::float_)
			.value("vec2", ShaderParameterType::vec2)
			.value("vec3", ShaderParameterType::vec3)
			.value("vec4", ShaderParameterType::vec4)
			.value("int", ShaderParameterType::int_)
			.value("ivec2", ShaderParameterType::ivec2)
			.value("ivec3", ShaderParameterType::ivec3)
			.value("ivec4", ShaderParameterType::ivec4)
			.value("bool", ShaderParameterType::bool_)
			.value("bvec2", ShaderParameterType::bvec2)
			.value("bvec3", ShaderParameterType::bvec3)
			.value("bvec4", ShaderParameterType::bvec4)
			.value("mat2", ShaderParameterType::mat2)
			.value("mat3", ShaderParameterType::mat3)
			.value("mat4", ShaderParameterType::mat4)
			.value("sampler1d", ShaderParameterType::sampler1d)
			.value("sampler2d", ShaderParameterType::sampler2d)
			.value("sampler3d", ShaderParameterType::sampler3d)
		;

		py::enum_<ContentHint>("ContentHint")
			.value("stream_content", ContentHint::stream_content)
			.value("static_content", ContentHint::static_content)
			.value("dynamic_content", ContentHint::dynamic_content)
		;


		py::enum_<ContentPacking>("ContentPacking")
			.value("uint8", ContentPacking::uint8)
			.value("int8", ContentPacking::int8)
			.value("uint16", ContentPacking::uint16)
			.value("int16", ContentPacking::int16)
			.value("uint32", ContentPacking::uint32)
			.value("int32", ContentPacking::int32)
			.value("float32", ContentPacking::float32)
			.value("uint8_3_3_2", ContentPacking::uint8_3_3_2)
			.value("uint8_2_3_3_rev", ContentPacking::uint8_2_3_3_rev)
			.value("uint16_5_6_5", ContentPacking::uint16_5_6_5)
			.value("uint16_5_6_5_rev", ContentPacking::uint16_5_6_5_rev)
			.value("uint16_4_4_4_4", ContentPacking::uint16_4_4_4_4)
			.value("uint16_4_4_4_4_rev", ContentPacking::uint16_4_4_4_4_rev)
			.value("uint16_5_5_5_1", ContentPacking::uint16_5_5_5_1)
			.value("uint16_1_5_5_5_rev", ContentPacking::uint16_1_5_5_5_rev)
			.value("uint32_8_8_8_8", ContentPacking::uint32_8_8_8_8)
			.value("uint32_8_8_8_8_rev", ContentPacking::uint32_8_8_8_8_rev)
			.value("uint32_10_10_10_2", ContentPacking::uint32_10_10_10_2)
			.value("uint32_2_10_10_10_rev", ContentPacking::uint32_2_10_10_10_rev)
		;

		py::enum_<PixelFormat>("PixelFormat")
			.value("depth_component", PixelFormat::depth_component)
			.value("depth_stencil", PixelFormat::depth_stencil)
			.value("red", PixelFormat::red)
			.value("rg", PixelFormat::rg)
			.value("rgb", PixelFormat::rgb)
			.value("rgba", PixelFormat::rgba)

			.value("r8", PixelFormat::r8)
			.value("r8_snorm", PixelFormat::r8_snorm)
			.value("r16", PixelFormat::r16)
			.value("r16_snorm", PixelFormat::r16_snorm)
			.value("rg8", PixelFormat::rg8)
			.value("rg8_snorm", PixelFormat::rg8_snorm)
			.value("rg16", PixelFormat::rg16)
			.value("rg16_snorm", PixelFormat::rg16_snorm)
			.value("r3_g3_b2", PixelFormat::r3_g3_b2)
			.value("rgb4", PixelFormat::rgb4)
			.value("rgb5", PixelFormat::rgb5)
			.value("rgb8", PixelFormat::rgb8)
			.value("rgb8_snorm", PixelFormat::rgb8_snorm)
			.value("rgb10", PixelFormat::rgb10)
			.value("rgb12", PixelFormat::rgb12)
			.value("rgb16_snorm", PixelFormat::rgb16_snorm)
			.value("rgba2", PixelFormat::rgba2)
			.value("rgba4", PixelFormat::rgba4)
			.value("rgb5_a1", PixelFormat::rgb5_a1)
			.value("rgba8", PixelFormat::rgba8)
			.value("rgba8_snorm", PixelFormat::rgba8_snorm)
			.value("rgb10_a2", PixelFormat::rgb10_a2)
			.value("rgb10_a2ui", PixelFormat::rgb10_a2ui)
			.value("rgba12", PixelFormat::rgba12)
			.value("rgba16", PixelFormat::rgba16)
			.value("srgb8", PixelFormat::srgb8)
			.value("srgb8_alpha8", PixelFormat::srgb8_alpha8)
			.value("r16f", PixelFormat::r16f)
			.value("rg16f", PixelFormat::rg16f)
			.value("rgb16f", PixelFormat::rgb16f)
			.value("rgba16f", PixelFormat::rgba16f)
			.value("r32f", PixelFormat::r32f)
			.value("rg32f", PixelFormat::rg32f)
			.value("rgb32f", PixelFormat::rgb32f)
			.value("rgba32f", PixelFormat::rgba32f)
			.value("r11f_g11f_b10f", PixelFormat::r11f_g11f_b10f)
			.value("rgb9_e5", PixelFormat::rgb9_e5)
			.value("r8i", PixelFormat::r8i)
			.value("r8ui", PixelFormat::r8ui)
			.value("r16i", PixelFormat::r16i)
			.value("r16ui", PixelFormat::r16ui)
			.value("r32i", PixelFormat::r32i)
			.value("r32ui", PixelFormat::r32ui)
			.value("rg8i", PixelFormat::rg8i)
			.value("rg8ui", PixelFormat::rg8ui)
			.value("rg16i", PixelFormat::rg16i)
			.value("rg16ui", PixelFormat::rg16ui)
			.value("rg32i", PixelFormat::rg32i)
			.value("rg32ui", PixelFormat::rg32ui)
			.value("rgb8i", PixelFormat::rgb8i)
			.value("rgb8ui", PixelFormat::rgb8ui)
			.value("rgb16i", PixelFormat::rgb16i)
			.value("rgb16ui", PixelFormat::rgb16ui)
			.value("rgb32i", PixelFormat::rgb32i)
			.value("rgb32ui", PixelFormat::rgb32ui)
			.value("rgba8i", PixelFormat::rgba8i)
			.value("rgba8ui", PixelFormat::rgba8ui)
			.value("rgba16i", PixelFormat::rgba16i)
			.value("rgba16ui", PixelFormat::rgba16ui)
			.value("rgba32i", PixelFormat::rgba32i)
			.value("rgba32ui", PixelFormat::rgba32ui)

			.value("compressed_red", PixelFormat::compressed_red)
			.value("compressed_rg", PixelFormat::compressed_rg)
			.value("compressed_rgb", PixelFormat::compressed_rgb)
			.value("compressed_rgba", PixelFormat::compressed_rgba)
			.value("compressed_srgb", PixelFormat::compressed_srgb)
			.value("compressed_srgb_alpha", PixelFormat::compressed_srgb_alpha)
			//.value("compressed_red_rgtc1", PixelFormat::compressed_red_rgtc1)
			//.value("compressed_signed_red_rgtc1", PixelFormat::compressed_signed_red_rgtc1)
			//.value("compressed_rg_rgtc2", PixelFormat::compressed_rg_rgtc2)
			//.value("compressed_signed_rg_rgtc2", PixelFormat::compressed_signed_rg_rgtc2)
			//.value("compressed_rgba_bptc_unorm", PixelFormat::compressed_rgba_bptc_unorm)
			//.value("compressed_srgb_alpha_bptc_unorm", PixelFormat::compressed_srgb_alpha_bptc_unorm)
			//.value("compressed_rgb_bptc_signed_float", PixelFormat::compressed_rgb_bptc_signed_float)
			//.value("compressed_rgb_bptc_unsigned_float", PixelFormat::compressed_rgb_bptc_unsigned_float)
		;

		py::enum_<DrawMode>("DrawMode")
			.value("points", DrawMode::points)
			.value("lines", DrawMode::lines)
			.value("line_strip", DrawMode::line_strip)
			.value("line_loop", DrawMode::line_loop)
			.value("triangles", DrawMode::triangles)
			.value("triangle_strip", DrawMode::triangle_strip)
			.value("triangle_fan", DrawMode::triangle_fan)
			.value("quads", DrawMode::quads)
			.value("quad_strip", DrawMode::quad_strip)
			.value("polygon", DrawMode::polygon)
		;

		py::enum_<BufferBit>("BufferBit")
			.value("color", BufferBit::color)
			.value("depth", BufferBit::depth)
			.value("stencil", BufferBit::stencil)
		;
	}

}}}
