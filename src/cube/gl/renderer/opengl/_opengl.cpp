#include "Exception.hpp"
#include "_opengl.hpp"

#include <map>

namespace cube { namespace gl { namespace renderer { namespace opengl {

	GLenum gl::_draw_mode_map[(size_t)DrawMode::_max_value] = {
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

	GLenum gl::_content_type_map[(size_t)ContentType::_max_value] = {
		GL_BYTE,    GL_UNSIGNED_BYTE,   GL_SHORT,   GL_UNSIGNED_SHORT,
		GL_INT,     GL_UNSIGNED_INT,    GL_FLOAT
	};
	GLenum gl::_content_packing_map[(size_t)ContentPacking::_max_value] = {
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

	GLenum gl::_content_kind_map[(size_t)ContentKind::_max_value]  = {
		GL_VERTEX_ARRAY, 0, GL_COLOR_ARRAY, GL_NORMAL_ARRAY,
		GL_TEXTURE_COORD_ARRAY,
		GL_TEXTURE_COORD_ARRAY,
		GL_TEXTURE_COORD_ARRAY,
	};

	GLenum gl::_content_hint_map[(size_t)ContentHint::_max_value] = {
		GL_STREAM_DRAW, GL_STATIC_DRAW, GL_DYNAMIC_DRAW,
	};

	GLenum gl::_shader_type_map[(size_t)ShaderType::_max_value] = {
		GL_FRAGMENT_SHADER,
		GL_VERTEX_SHADER,
	};


	GLenum gl::_pixel_format_map[(size_t)PixelFormat::_max_value] = {

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

	void gl::_check_error(char const* function_)
	{
		GLenum code = glGetError();

		if (code == GL_NO_ERROR)
			return;

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

		std::string error, description;
		auto it = errors.find(code);
		if (it == errors.end())
		{
			error = etc::to_string("Unknown error code", code);
			description = "Not documented error.";
		}
		else
		{
			error = it->second.first;
			description = it->second.second;
		}

		std::string function = (
			function_ != nullptr ? function_ : "(anonymous function)"
		);
        throw Exception{
			std::string{function} + ": " + error + ": " + description
		};
    }

}}}} // !cube::gl::opengl
