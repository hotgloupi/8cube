#include "font.hpp"

#include "exception.hpp"
#include "vector.hpp"

#include <etc/log.hpp>

#include <wrappers/freetype.hpp>

ETC_LOG_COMPONENT("cube.gl.font.Font");

namespace cube { namespace gl { namespace font {

	typedef exception::Exception Exception;

	namespace { namespace freetype {

		struct Library
		{
			::FT_Library    handle;
			bool            initialized;

			Library()
				: handle(nullptr)
				, initialized{false}
			{
				if (auto error = ::FT_Init_FreeType(&this->handle))
					ETC_LOG.warn("FT_Init_FreeType error:", error);
				else
					this->initialized = true;
			}
			~Library()
			{
				if (this->initialized)
					::FT_Done_FreeType(this->handle);
			}
		};

		unsigned int next_p2(unsigned int n)
		{
			assert(n * 2u > n);
			unsigned int p2 = 2;
			while (n > p2) p2 *= 2;
			return p2;
		}


# define FT_CALL(name, ...)                                                   \
	do {                                                                      \
		auto error = ::FT_ ## name(__VA_ARGS__);                              \
		if (error)                                                            \
			throw Exception{                                                  \
				etc::to_string("FT_" #name, "error:", error)                  \
			};                                                                \
	} while (false)                                                           \
/**/

		struct Face
		{
			::FT_Face handle;

			Face(freetype::Library& library,
			     std::string const& name,
			     unsigned int size)
				: handle(nullptr)
			{
				FT_CALL(
					New_Face,
					library.handle,
					name.c_str(),
					0,
					&this->handle
				);

				FT_CALL(
					Set_Char_Size,
					this->handle,
					static_cast<FT_F26Dot6>(size * 64),
					static_cast<FT_F26Dot6>(size * 64),
					96,
					96
				);

				FT_CALL(Select_Charmap, this->handle, FT_ENCODING_UNICODE);
			}

			~Face()
			{
				::FT_Done_Face(this->handle);
			}
		};

		struct Glyph
		{
			::FT_Glyph handle;
			::FT_Bitmap bitmap;

			Glyph(Face& face, uint16_t charcode)
				: handle(nullptr)
				, bitmap()
			{
				auto index = ::FT_Get_Char_Index(face.handle, charcode);
				if (index == 0)
					throw Exception{
						"Couldn't find glyph for charcode " + etc::to_string(charcode)
					};
				FT_CALL(Load_Glyph, face.handle, index, FT_LOAD_RENDER);
				FT_CALL(Get_Glyph, face.handle->glyph, &this->handle);
				FT_CALL(
					Glyph_To_Bitmap,
					&this->handle,
					FT_RENDER_MODE_NORMAL,      // render mode
					nullptr,                    // origin == (0, 0)
					1                           // destroy original == true
				);
				this->bitmap = ((::FT_BitmapGlyph) this->handle)->bitmap;
			}
			~Glyph()
			{
				::FT_Done_Glyph(this->handle);
			}
		};

	}} //!anonymous::freetype

	struct Font::Impl
	{
	private:
		static freetype::Library _library;

	public:
		Impl()
		{
			if (!_library.initialized)
				throw Exception{"FreeType library is not initialized"};
		}
	};

	freetype::Library Font::Impl::_library{};

	Font::Font(std::string const& name,
	           size_t size)
		: _impl(new Impl)
	{
	}

	Font::~Font()
	{

	}

}}}
