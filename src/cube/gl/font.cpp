#include "font.hpp"

#include "exception.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/Texture.hpp"
#include "vector.hpp"

#include <etc/log.hpp>

#include <wrappers/freetype.hpp>

#include <unordered_map>

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

		struct GlyphMap
		{
		private:
			typedef std::unique_ptr<Glyph> GlyphPtr;
			typedef std::unique_ptr<renderer::Texture> TexturePtr;
		private:
			std::unordered_map<uint16_t, GlyphPtr>  _glyphs;
			bool                                    _full;
			Face&                                   _face;
			TexturePtr                              _texture;

		public:
			GlyphMap(Face& face, TexturePtr&& texture)
				: _glyphs{}
				, _full{false}
				, _face(face)
				, _texture{std::move(texture)}
			{}

		private:
			static inline
			unsigned int next_p2(unsigned int n)
			{
				unsigned int i = 2;
				while (i < n) i *= 2;
				return i;
			}

			Glyph& _gen_char(uint16_t c)
			{
				_glyphs[c].reset(new Glyph(_face, c));
				Glyph& glyph = *_glyphs[c].get();

				std::vector<uint8_t> data(glyph.bitmap.width * glyph.bitmap.rows);

				::memcpy(&data[0], glyph.bitmap.buffer, data.size());
				_texture->set_data(0, 0,
				                   glyph.bitmap.width, glyph.bitmap.rows,
				                   renderer::PixelFormat::red,
				                   renderer::ContentPacking::uint8,
				                   &data[0]);
				return glyph;
			}

		};

	}} //!anonymous::freetype

	struct Font::Impl
	{
	private:
		// unique freetype library instance
		static freetype::Library    _library;

	public:
		renderer::Renderer&         renderer;
	private:
		freetype::Face              _face;
		freetype::GlyphMap          _glyphs;

	public:
		Impl(renderer::Renderer& renderer,
		     std::string const& name,
		     unsigned int size)
			: renderer(renderer)
			, _face{_library, name, size}
			, _glyphs{_face, renderer.new_texture(
				renderer::PixelFormat::red,
				1024,
				1024,
				renderer::PixelFormat::red,
				renderer::ContentPacking::uint8,
				nullptr
			)}
		{
			if (!_library.initialized)
				throw Exception{"FreeType library is not initialized"};
		}


	};

	freetype::Library Font::Impl::_library{};

	Font::Font(renderer::Renderer& renderer,
	           std::string const& name,
	           unsigned int size)
		: _impl{new Impl{renderer, name, size}}
	{
	}

	Font::~Font()
	{

	}

}}}
