#ifndef  CUBE_GL_RENDERER_CONSTANTS_HPP
# define CUBE_GL_RENDERER_CONSTANTS_HPP

# include <cube/api.hpp>
# include <cube/gl/matrix.hpp>

# include <cstdint>
# include <cstddef>
# include <iosfwd>

namespace cube { namespace gl { namespace renderer {

	/**
	 * Names of available renderer.
	 */
	enum class Name
	{
		OpenGL = 1,
		DirectX = 2,
	};

	/**
	 * The rendering mode.
	 */
	enum class Mode
	{
		none = 0,
		_2d,
		_3d,

		_max_value
	};
	CUBE_API
	std::ostream& operator <<(std::ostream& out, Mode mode);

	/**
	 * Matrix type used for every transformation.
	 */
	typedef ::cube::gl::matrix::Matrix44f      matrix_type;

	enum class MatrixKind
	{
		model = 0,
		view,
		projection,
		mvp,

		_max_value
	};
	CUBE_API
	std::ostream& operator <<(std::ostream& out, MatrixKind kind);

	enum class BufferBit : int
	{
		none    = 0,
		color   = 1,
		depth   = 2,
		stencil = 4,

		_max_value
	};

	inline
	BufferBit operator &(BufferBit a, BufferBit b)
	{
		return static_cast<BufferBit>((int) a & (int) b);
	}

	inline
	BufferBit operator |(BufferBit a, BufferBit b)
	{
		return static_cast<BufferBit>((int) a | (int) b);
	}

	enum class ShaderType
	{
		fragment = 0,
		vertex,

		_max_value
	};
	CUBE_API
	std::ostream& operator <<(std::ostream& out, ShaderType const type);

	enum class ShaderParameterType
	{
		float_,
		vec2, vec3, vec4,

		int_,
		ivec2, ivec3, ivec4,

		bool_,
		bvec2, bvec3, bvec4,

		mat2, mat3, mat4,
		sampler1d, sampler2d, sampler3d,
		samplerCube,
		sampler1DShadow, sampler2DShadow,
	};
	// XXX
	// CUBE_API
	// std::ostream&
	// operator <<(std::ostream& out, ShaderParameterType const type);

	enum class ShaderLanguage
	{
		glsl = 0,
		hlsl,
		cg,

		_max_value
	};
	// XXX
	//std::ostream& operator <<(std::ostream& out, ShaderLanguage const lang);

	enum class DrawMode
	{
		points,
		lines,
		line_strip,
		line_loop,
		triangles,
		triangle_strip,
		triangle_fan,
		quads,
		quad_strip,
		polygon,

		_max_value
	};
	CUBE_API
	std::ostream& operator<<(std::ostream& out, DrawMode const mode);

	enum class ContentType
	{
		int8 = 0,
		uint8,
		int16,
		uint16,
		int32,
		uint32,
		float32,

		_max_value,
	};
	CUBE_API
	std::ostream& operator <<(std::ostream& out, ContentType const type);

	enum class ContentPacking
	{
		uint8,
		int8,
		uint16,
		int16,
		uint32,
		int32,
		float32,
		uint8_3_3_2,
		uint8_2_3_3_rev,
		uint16_5_6_5,
		uint16_5_6_5_rev,
		uint16_4_4_4_4,
		uint16_4_4_4_4_rev,
		uint16_5_5_5_1,
		uint16_1_5_5_5_rev,
		uint32_8_8_8_8,
		uint32_8_8_8_8_rev,
		uint32_10_10_10_2,
		uint32_2_10_10_10_rev,

		_max_value
	};

	enum class ContentHint
	{
		stream_content = 0,
		static_content,
		dynamic_content,

		_max_value
	};
	CUBE_API
	std::ostream& operator <<(std::ostream& out, ContentHint const hint);

	enum class ContentKind
	{
		vertex = 0,
		index,
		color,
		normal,
		tex_coord0,
		tex_coord1,
		tex_coord2,

		_max_value
	};
	CUBE_API
	std::ostream& operator <<(std::ostream& out, ContentKind const kind);

	enum class PixelFormat
	{
		depth_component = 0,
		depth_stencil,
		red,
		rg,
		rgb,
		rgba,

		r8,
		r8_snorm,
		r16,
		r16_snorm,
		rg8,
		rg8_snorm,
		rg16,
		rg16_snorm,
		r3_g3_b2,
		rgb4,
		rgb5,
		rgb8,
		rgb8_snorm,
		rgb10,
		rgb12,
		rgb16_snorm,
		rgba2,
		rgba4,
		rgb5_a1,
		rgba8,
		rgba8_snorm,
		rgb10_a2,
		rgb10_a2ui,
		rgba12,
		rgba16,
		srgb8,
		srgb8_alpha8,
		r16f,
		rg16f,
		rgb16f,
		rgba16f,
		r32f,
		rg32f,
		rgb32f,
		rgba32f,
		r11f_g11f_b10f,
		rgb9_e5,
		r8i,
		r8ui,
		r16i,
		r16ui,
		r32i,
		r32ui,
		rg8i,
		rg8ui,
		rg16i,
		rg16ui,
		rg32i,
		rg32ui,
		rgb8i,
		rgb8ui,
		rgb16i,
		rgb16ui,
		rgb32i,
		rgb32ui,
		rgba8i,
		rgba8ui,
		rgba16i,
		rgba16ui,
		rgba32i,
		rgba32ui,

		compressed_red,
		compressed_rg,
		compressed_rgb,
		compressed_rgba,
		compressed_srgb,
		compressed_srgb_alpha,
		//compressed_red_rgtc1,
		//compressed_signed_red_rgtc1,
		//compressed_rg_rgtc2,
		//compressed_signed_rg_rgtc2,
		//compressed_rgba_bptc_unorm,
		//compressed_srgb_alpha_bptc_unorm,
		//compressed_rgb_bptc_signed_float,
		//compressed_rgb_bptc_unsigned_float,

		_max_value
	};
	CUBE_API
	std::ostream& operator <<(std::ostream& out, PixelFormat const format);

	template<ContentType type> struct ContentTypeSize;
	template<> struct ContentTypeSize<ContentType::int8>
		{ static size_t const value = sizeof(int8_t); };
	template<> struct ContentTypeSize<ContentType::uint8>
		{ static size_t const value = sizeof(uint8_t); };
	template<> struct ContentTypeSize<ContentType::int16>
		{ static size_t const value = sizeof(int16_t); };
	template<> struct ContentTypeSize<ContentType::uint16>
		{ static size_t const value = sizeof(uint16_t); };
	template<> struct ContentTypeSize<ContentType::int32>
		{ static size_t const value = sizeof(int32_t); };
	template<> struct ContentTypeSize<ContentType::uint32>
		{ static size_t const value = sizeof(uint32_t); };
	template<> struct ContentTypeSize<ContentType::float32>
		{ static size_t const value = sizeof(float); };

	template<typename T> struct MakeContentType;
	template<> struct MakeContentType<int8_t>
		{ static ContentType const value = ContentType::int8; };
	template<> struct MakeContentType<uint8_t>
		{ static ContentType const value = ContentType::uint8; };
	template<> struct MakeContentType<int16_t>
		{ static ContentType const value = ContentType::int16; };
	template<> struct MakeContentType<uint16_t>
		{ static ContentType const value = ContentType::uint16; };
	template<> struct MakeContentType<int32_t>
		{ static ContentType const value = ContentType::int32; };
	template<> struct MakeContentType<uint32_t>
		{ static ContentType const value = ContentType::uint32; };
	template<> struct MakeContentType<float>
		{ static ContentType const value = ContentType::float32; };

	inline size_t get_content_type_size(ContentType type)
	{
		switch (type)
		{
		case ContentType::int8:
			return ContentTypeSize<ContentType::int8>::value;
		case ContentType::uint8:
			return ContentTypeSize<ContentType::uint8>::value;
		case ContentType::int16:
			return ContentTypeSize<ContentType::int16>::value;
		case ContentType::uint16:
			return ContentTypeSize<ContentType::uint16>::value;
		case ContentType::int32:
			return ContentTypeSize<ContentType::int32>::value;
		case ContentType::uint32:
			return ContentTypeSize<ContentType::uint32>::value;
		case ContentType::float32:
			return ContentTypeSize<ContentType::float32>::value;
		default:
			throw false;
		}
	}

}}} // !cube::gl::renderer

#endif
