#include "font.hpp"

#include "exception.hpp"
#include "renderer/Painter.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/ShaderProgram.hpp"
#include "renderer/Texture.hpp"
#include "renderer/VertexBuffer.hpp"
#include "vector.hpp"

#include <etc/log.hpp>

#include <wrappers/freetype.hpp>

#include <unordered_map>

ETC_LOG_COMPONENT("cube.gl.font.Font");

namespace cube { namespace gl { namespace font {

	typedef exception::Exception Exception;

	// FreeType related objects
	namespace { namespace freetype {

		struct Library
		{
			::FT_Library    handle;
			bool            initialized;

			Library()
				: handle(nullptr)
				, initialized{false}
			{
				ETC_TRACE.debug("New FreeType library");
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

				ETC_TRACE.debug("New FreeType Face", name);
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
			::FT_Glyph          handle;
			::FT_Bitmap         bitmap;
			uint32_t            id;
			vector::Vector2f    position;
			vector::Vector2f    size;

			Glyph(Face& face, uint32_t charcode, uint32_t id)
				: handle(nullptr)
				, bitmap()
				, id(id)
			{
				ETC_TRACE.debug("New FreeType Glyph of", charcode);
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
				FT_BBox bbox;
				::FT_Glyph_Get_CBox(this->handle, FT_GLYPH_BBOX_PIXELS, &bbox);
			}
			~Glyph()
			{
				::FT_Done_Glyph(this->handle);
			}
		};

		struct GlyphMap
		{
		private:
			typedef std::unique_ptr<Glyph>                  GlyphPtr;
			typedef std::unique_ptr<renderer::Texture>      TexturePtr;
			typedef std::unique_ptr<renderer::VertexBuffer> VertexBufferPtr;
		private:
			renderer::Renderer&                     _renderer;
			std::unordered_map<uint32_t, GlyphPtr>  _glyphs;
			bool                                    _full;
			Face&                                   _face;
			vector::Vector2f                        _texture_size;
			TexturePtr                              _texture;
			std::vector<vector::Vector2f>           _coords;
			std::vector<vector::Vector2f>           _tex_coords;
			VertexBufferPtr                         _vertex_buffer;
			uint32_t                                _next_id;
			vector::Vector2f                        _pen;
			etc::size_type                          _max_line_height;
		public:
			GlyphMap(Face& face, renderer::Renderer& renderer)
				: _renderer(renderer)
				, _glyphs{}
				, _full{false}
				, _face(face)
				, _texture_size{1024, 1024}
				, _texture{renderer.new_texture(
					renderer::PixelFormat::rgba,
					_texture_size.x,
					_texture_size.y,
					renderer::PixelFormat::rgba,
					renderer::ContentPacking::uint8,
					nullptr
				)}
				, _coords{}
				, _tex_coords{}
				, _vertex_buffer{}
				, _next_id{0}
				, _pen{0, 0}
				, _max_line_height{0}
			{
				ETC_TRACE.debug("New GlyphMap");
			}

			renderer::Texture& texture() { return *_texture.get(); }
			renderer::VertexBuffer& vertex_buffer()
			{
				if (_vertex_buffer == nullptr)
				{
					_vertex_buffer = _renderer.new_vertex_buffer();
					_vertex_buffer->push_static_content(
						renderer::ContentKind::tex_coord0,
						&_tex_coords[0],
						_tex_coords.size()
					);
					_vertex_buffer->push_static_content(
						renderer::ContentKind::vertex,
						&_coords[0],
						_coords.size()
					);
					_vertex_buffer->finalize();
				}
				return *_vertex_buffer.get();
			}
			Glyph& get_glyph(uint32_t c)
			{
				auto it = _glyphs.find(c);
				if (it != _glyphs.end())
					return *it->second.get();
				else
					return _gen_glyph(c);
			}
		private:
			static inline
			unsigned int next_p2(unsigned int n)
			{
				unsigned int i = 2;
				while (i < n) i *= 2;
				return i;
			}

			Glyph& _gen_glyph(uint32_t c)
			{
				auto dbg = ETC_LOG.debug("Generate glyph", c, "at", _pen, "with id =", _next_id);
				_vertex_buffer.reset(); // tex coord buffer needs to be regenerated

				_glyphs[c].reset(new Glyph(_face, c, _next_id++));
				Glyph& glyph = *_glyphs[c].get();

				glyph.position = _pen;


				if (glyph.size.y > _max_line_height)
					_max_line_height = glyph.size.y;

				dbg("glyph size:", glyph.bitmap.width, ',', glyph.bitmap.rows);
				_coords.push_back(_pen);
				_coords.emplace_back(_pen.x + glyph.bitmap.width, _pen.y);
				_coords.emplace_back(_pen.x + glyph.bitmap.width, _pen.y + glyph.bitmap.rows);
				_coords.emplace_back(_pen.x, _pen.y + glyph.bitmap.rows);

				_tex_coords.emplace_back(
					_pen.x / _texture_size.x,
					_pen.y / _texture_size.y
				);
				_tex_coords.emplace_back(
					(_pen.x + glyph.bitmap.width) / _texture_size.x,
					_pen.y / _texture_size.y
				);
				_tex_coords.emplace_back(
					(_pen.x + glyph.bitmap.width) / _texture_size.x,
					(_pen.y + glyph.bitmap.rows) / _texture_size.y
				);
				_tex_coords.emplace_back(
					_pen.x / _texture_size.x,
					(_pen.y + glyph.bitmap.rows) / _texture_size.y
				);
				char buffer[glyph.bitmap.width * glyph.bitmap.rows * 4];
				memset(buffer, 12, glyph.bitmap.width* glyph.bitmap.rows * 4);
				for (int i = 0; i < glyph.bitmap.width; ++i)
					for (int j = 0; j < glyph.bitmap.rows; ++j)
				{
					char col = glyph.bitmap.buffer[i + j * glyph.bitmap.width];
					buffer[(i + j * glyph.bitmap.width) * 4 + 0] = col;
					buffer[(i + j * glyph.bitmap.width) * 4 + 1] = col;
					buffer[(i + j * glyph.bitmap.width) * 4 + 2] = col;
					buffer[(i + j * glyph.bitmap.width) * 4 + 3] = 1;
				}
				_texture->set_data(_pen.x, _pen.y,
				                   glyph.bitmap.width, glyph.bitmap.rows,
				                   renderer::PixelFormat::rgba,
				                   renderer::ContentPacking::uint8,
				                   buffer);
				_pen.x += glyph.bitmap.width + 2;
				_pen.y += 0; //glyph.bitmap.rows;
				return glyph;
			}
		};

	}} //!anonymous::freetype

	///////////////////////////////////////////////////////////////////////////
	// Font implem class

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
			, _glyphs{_face, renderer}
		{
			if (!_library.initialized)
				throw Exception{"FreeType library is not initialized"};
		}

		template<typename CharType>
		uint32_t get_index(CharType c)
		{
			return _glyphs.get_glyph(c).id;
		}

		void bind(renderer::Painter& painter,
		          renderer::ShaderProgramParameter& param)
		{
			param = _glyphs.texture();
			painter.bind(_glyphs.texture());
			painter.bind(_glyphs.vertex_buffer());
		}

		void unbind(renderer::Painter& painter)
		{
			painter.unbind(_glyphs.texture());
			painter.unbind(_glyphs.vertex_buffer());
		}
	};

	freetype::Library Font::Impl::_library{};


	///////////////////////////////////////////////////////////////////////////
	// Font class

	Font::Font(renderer::Renderer& renderer,
	           std::string const& name,
	           unsigned int size)
		: _impl{new Impl{renderer, name, size}}
	{}

	Font::~Font()
	{}

	template<typename CharType>
	std::unique_ptr<renderer::VertexBuffer>
	Font::generate_text(std::basic_string<CharType> const& str)
	{
		std::vector<uint32_t> indices;//(str.size());
		for (CharType c : str)
		{
			indices.push_back(_impl->get_index(c) * 4 + 0);
			indices.push_back(_impl->get_index(c) * 4 + 1);
			indices.push_back(_impl->get_index(c) * 4 + 2);
			indices.push_back(_impl->get_index(c) * 4 + 3);
		}
		auto ib = _impl->renderer.new_index_buffer();
		ib->push_static_content(
			renderer::ContentKind::index,
			&indices[0],
			indices.size()
		);
		ib->finalize();
		return ib;
	}

	template
	std::unique_ptr<renderer::VertexBuffer>
	Font::generate_text<char>(std::basic_string<char> const& str);

	template
	std::unique_ptr<renderer::VertexBuffer>
	Font::generate_text<wchar_t>(std::basic_string<wchar_t> const& str);

	void Font::bind(renderer::Painter& painter,
	                renderer::ShaderProgramParameter& param)
	{
		_impl->bind(painter, param);
	}

	void Font::unbind(renderer::Painter& painter)
	{
		_impl->unbind(painter);
	}

}}}
