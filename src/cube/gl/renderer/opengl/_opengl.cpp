#include "Exception.hpp"
#include "_opengl.hpp"

#include <etc/abort.hpp>

#include <map>

namespace cube { namespace gl { namespace renderer { namespace opengl {

	std::array<GLenum, gl::_draw_modes> gl::_draw_mode_map{
		GL_POINTS,
		GL_LINES,
		GL_LINE_STRIP,
		GL_LINE_LOOP,
		GL_TRIANGLES,
		GL_TRIANGLE_STRIP,
		GL_TRIANGLE_FAN,
		GL_QUADS,
		GL_QUAD_STRIP,
		GL_POLYGON,
	};

	std::array<GLenum, gl::_content_types> gl::_content_type_map{
		GL_BYTE,    GL_UNSIGNED_BYTE,   GL_SHORT,   GL_UNSIGNED_SHORT,
		GL_INT,     GL_UNSIGNED_INT,    GL_FLOAT
	};
	std::array<GLenum, gl::_content_packings> gl::_content_packing_map{
		GL_UNSIGNED_BYTE,
		GL_BYTE,
		GL_UNSIGNED_SHORT,
		GL_SHORT,
		GL_UNSIGNED_INT,
		GL_INT,
		GL_FLOAT,
		GL_UNSIGNED_BYTE_3_3_2,
		GL_UNSIGNED_BYTE_2_3_3_REV,
		GL_UNSIGNED_SHORT_5_6_5,
		GL_UNSIGNED_SHORT_5_6_5_REV,
		GL_UNSIGNED_SHORT_4_4_4_4,
		GL_UNSIGNED_SHORT_4_4_4_4_REV,
		GL_UNSIGNED_SHORT_5_5_5_1,
		GL_UNSIGNED_SHORT_1_5_5_5_REV,
		GL_UNSIGNED_INT_8_8_8_8,
		GL_UNSIGNED_INT_8_8_8_8_REV,
		GL_UNSIGNED_INT_10_10_10_2,
		GL_UNSIGNED_INT_2_10_10_10_REV,
	};

	GLenum gl::get_content_kind(renderer::ContentKind value)
	{
		typedef renderer::ContentKind K;
		static int const kinds = (int) K::_max_value;
		static std::array<GLenum, kinds> content_kind_map = [] {
			std::array<GLenum, kinds> res;
			res[(int)K::vertex] = GL_VERTEX_ARRAY;
			res[(int)K::index] = 0;
			res[(int)K::normal] = GL_NORMAL_ARRAY;

			res[(int)K::color] = GL_COLOR_ARRAY;
			for (int i = (int) K::color0; i < (int) K::_max_color; ++i)
				res[i] = GL_COLOR_ARRAY;

			res[(int)K::tex_coord] = GL_TEXTURE_COORD_ARRAY;
			for (int i = (int) K::tex_coord0; i < (int) K::_max_tex_coord; ++i)
				res[i] = GL_TEXTURE_COORD_ARRAY;

			int n;
			glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &n);
			if (n > 0 && n < kinds)
			{
				etc::print("max vertex attribs", n);
				etc::abort(
					"Too many attribute :" +
					std::to_string(n) + " < " + std::to_string(kinds)
				);
			}
			return res;
		}();
		return content_kind_map[(int)value];
	}

	std::array<GLenum, gl::_content_hints> gl::_content_hint_map{
		GL_STREAM_DRAW, GL_STATIC_DRAW, GL_DYNAMIC_DRAW,
	};

	std::array<GLenum, gl::_shader_types> gl::_shader_type_map{
		GL_FRAGMENT_SHADER,
		GL_VERTEX_SHADER,
	};


	std::array<GLenum, gl::_pixel_formats> gl::_pixel_format_map{

		//////////// Table 1. Base Internal Formats
		// Base Internal Format     RGBA, Depth and Stencil Values  Internal Components
		GL_DEPTH_COMPONENT,      // Depth                           D
		GL_DEPTH_STENCIL,        // Depth, Stencil                  D, S
		GL_RED,                  // Red                             R
		GL_RG,                   // Red, Green                      R, G
		GL_RGB,                  // Red, Green, Blue                R, G, B
		GL_RGBA,                 // Red, Green, Blue, Alpha         R, G, B, A

		//////////// Table 2. Sized Internal Formats
		// Sized Internal Format     Base Internal Format      Red Bits   Green Bits  Blue Bits Alpha Bits Shared Bits
		GL_R8,                    // GL_RED8
		GL_R8_SNORM,              // GL_RED                    s8
		GL_R16,                   // GL_RED                    16
		GL_R16_SNORM,             // GL_RED                    s16
		GL_RG8,                   // GL_RG                     8          8
		GL_RG8_SNORM,             // GL_RG                     s8         s8
		GL_RG16,                  // GL_RG                     16         16
		GL_RG16_SNORM,            // GL_RG                     s16        s16
		GL_R3_G3_B2,              // GL_RGB                    3          3           2
		GL_RGB4,                  // GL_RGB                    4          4           4
		GL_RGB5,                  // GL_RGB                    5          5           5
		GL_RGB8,                  // GL_RGB                    8          8           8
		GL_RGB8_SNORM,            // GL_RGB                    s8         s8          s8
		GL_RGB10,                 // GL_RGB                    10         10          10
		GL_RGB12,                 // GL_RGB                    12         12          12
		GL_RGB16_SNORM,           // GL_RGB                    16         16          16
		GL_RGBA2,                 // GL_RGB                    2          2           2         2
		GL_RGBA4,                 // GL_RGB                    4          4           4         4
		GL_RGB5_A1,               // GL_RGBA                   5          5           5         1
		GL_RGBA8,                 // GL_RGBA                   8          8           8         8
		GL_RGBA8_SNORM,           // GL_RGBA                   s8         s8          s8        s8
		GL_RGB10_A2,              // GL_RGBA                   10         10          10        2
		GL_RGB10_A2UI,            // GL_RGBA                   ui10       ui10        ui10      ui2
		GL_RGBA12,                // GL_RGBA                   12         12          12        12
		GL_RGBA16,                // GL_RGBA                   16         16          16        16
		GL_SRGB8,                 // GL_RGB                    8          8           8
		GL_SRGB8_ALPHA8,          // GL_RGBA                   8          8           8         8
		GL_R16F,                  // GL_RED                    f16
		GL_RG16F,                 // GL_RG                     f16        f16
		GL_RGB16F,                // GL_RGB                    f16        f16         f16
		GL_RGBA16F,               // GL_RGBA                   f16        f16         f16       f16
		GL_R32F,                  // GL_RED                    f32
		GL_RG32F,                 // GL_RG                     f32        f32
		GL_RGB32F,                // GL_RGB                    f32        f32         f32
		GL_RGBA32F,               // GL_RGBA                   f32        f32         f32       f32
		GL_R11F_G11F_B10F,        // GL_RGB                    f11        f11         f10
		GL_RGB9_E5,               // GL_RGB                    9          9           9         5
		GL_R8I,                   // GL_RED                    i8
		GL_R8UI,                  // GL_RED                    ui8
		GL_R16I,                  // GL_RED                    i16
		GL_R16UI,                 // GL_RED                    ui16
		GL_R32I,                  // GL_RED                    i32
		GL_R32UI,                 // GL_RED                    ui32
		GL_RG8I,                  // GL_RG                     i8         i8
		GL_RG8UI,                 // GL_RG                     ui8        ui8
		GL_RG16I,                 // GL_RG                     i16        i16
		GL_RG16UI,                // GL_RG                     ui16       ui16
		GL_RG32I,                 // GL_RG                     i32        i32
		GL_RG32UI,                // GL_RG                     ui32       ui32
		GL_RGB8I,                 // GL_RGB                    i8         i8          i8
		GL_RGB8UI,                // GL_RGB                    ui8        ui8         ui8
		GL_RGB16I,                // GL_RGB                    i16        i16         i16
		GL_RGB16UI,               // GL_RGB                    ui16       ui16        ui16
		GL_RGB32I,                // GL_RGB                    i32        i32         i32
		GL_RGB32UI,               // GL_RGB                    ui32       ui32        ui32
		GL_RGBA8I,                // GL_RGBA                   i8         i8          i8        i8
		GL_RGBA8UI,               // GL_RGBA                   ui8        ui8         ui8       ui8
		GL_RGBA16I,               // GL_RGBA                   i16        i16         i16       i16
		GL_RGBA16UI,              // GL_RGBA                   ui16       ui16        ui16      ui16
		GL_RGBA32I,               // GL_RGBA                   i32        i32         i32       i32
		GL_RGBA32UI,              // GL_RGBA                   ui32       ui32        ui32      ui32

		/////// Table 3. Compressed Internal Formats
		//Compressed Internal Format              Base Internal Format      Type
		GL_COMPRESSED_RED,                     // GL_RED                    Generic
		GL_COMPRESSED_RG,                      // GL_RG                     Generic
		GL_COMPRESSED_RGB,                     // GL_RGB                    Generic
		GL_COMPRESSED_RGBA,                    // GL_RGBA                   Generic
		GL_COMPRESSED_SRGB,                    // GL_RGB                    Generic
		GL_COMPRESSED_SRGB_ALPHA,              // GL_RGBA                   Generic
		//GL_COMPRESSED_RED_RGTC1,               // GL_RED                    Specific
		//GL_COMPRESSED_SIGNED_RED_RGTC1,        // GL_RED                    Specific
		//GL_COMPRESSED_RG_RGTC2,                // GL_RG                     Specific
		//GL_COMPRESSED_SIGNED_RG_RGTC2,         // GL_RG                     Specific
		//GL_COMPRESSED_RGBA_BPTC_UNORM,         // GL_RGBA                   Specific
		//GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,   // GL_RGBA                   Specific
		//GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,   // GL_RGB                    Specific
		//GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT, // GL_RGB                    Specific
	};

	static std::map<GLenum, std::pair<std::string, std::string>> errors{
#define _ERR(enum, str)                                                       \
	{enum, {#enum, str "."}}                                                  \
	/**/
			_ERR(GL_INVALID_ENUM, "Wrong enum value"),
			_ERR(GL_INVALID_VALUE, "Wrong value"),
			_ERR(GL_INVALID_OPERATION, "Wrong operation"),
			_ERR(GL_STACK_OVERFLOW, "Stack overflow"),
			_ERR(GL_OUT_OF_MEMORY, "Out of memory"),
			_ERR(GL_INVALID_FRAMEBUFFER_OPERATION_EXT,
				 "Wrong operation on a frame buffer"),
#undef _ERR
	};

	template<gl::ThrowPolicy error_policy>
	void gl::_check_error(char const* function_) noexcept(error_policy == gl::no_throw)
	{
		GLenum code = glGetError();

		if (code == GL_NO_ERROR)
			return;

		std::string error = (function_ != nullptr ? function_ : "(nil)");
		error += ": ";
		auto it = errors.find(code);
		if (it == errors.end())
		{
			error += "Unknown error code (" + etc::to_string(code) + "):";
			error += "Undocumented error.";
		}
		else
		{
			error += it->second.first + ": " + it->second.second;
		}

		if (error_policy == can_throw)
			throw Exception{error};
		ETC_LOG.error("Unchecked OpenGL error:", error);
    }

	template
	void gl::_check_error<gl::no_throw>(char const* function_) noexcept(true);
	template
	void gl::_check_error<gl::can_throw>(char const* function_) noexcept(false);

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

	renderer::PixelFormat gl::to_pixel_format(GLenum value) noexcept
	{
		size_t i = 0;
		while (i < ARRAY_SIZE(_pixel_format_map) &&
		       _pixel_format_map[i] != value)
		     ++i;
		return static_cast<renderer::PixelFormat>(i);
	}

	renderer::ContentPacking gl::to_content_packing(GLenum value) noexcept
	{
		size_t i = 0;
		while (i < ARRAY_SIZE(_content_packing_map) &&
		       _content_packing_map[i] != value)
		     ++i;
		return static_cast<renderer::ContentPacking>(i);
	}

}}}} // !cube::gl::opengl
