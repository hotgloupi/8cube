#ifndef  CUBE_GL_RENDERER_CONSTANTS_HPP
# define CUBE_GL_RENDERER_CONSTANTS_HPP

namespace cube { namespace gl { namespace renderer {

	enum class BufferBit : int
	{
    none    = 0,
		color   = 1,
		depth   = 2,
		stencil = 4,
	};

  inline BufferBit operator &(BufferBit a, BufferBit b)
  {
    return (BufferBit)((int) a & (int) b);
  }

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

		_max_value,
	};

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

	enum class ContentHint
	{
		stream_content = 0,
		static_content,
		dynamic_content,

		_max_value,
	};

	enum class ContentKind
	{
		vertex = 0,
		index,
		color,
		normal,
		tex_coord0,
		tex_coord1,
		tex_coord2,

		_max_value,
	};

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

