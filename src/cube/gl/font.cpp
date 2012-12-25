#include "font.hpp"

#include "exception.hpp"
#include "renderer/Painter.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/ShaderProgram.hpp"
#include "renderer/Texture.hpp"
#include "renderer/VertexBuffer.hpp"
#include "vector.hpp"

#include <etc/log.hpp>
#include <etc/memory.hpp>

#include <wrappers/freetype.hpp>

#include <unordered_map>

ETC_LOG_COMPONENT("cube.gl.font.Font");

namespace cube { namespace gl { namespace font {

	typedef exception::Exception Exception;

	// FreeType related objects
	namespace { namespace freetype {

		/**
		 * @brief singleton instance for Freetype library.
		 */
		struct Library
		{
			::FT_Library    handle;
			bool            initialized;

			static Library& instance()
			{
				static Library library{};
				if (!library.initialized)
					throw Exception{"Cannot initialize Freetype library"};
				return library;
			}

		private:
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

# define FT_CALL(name, ...)                                                   \
	do {                                                                      \
		auto error = ::FT_ ## name(__VA_ARGS__);                              \
		if (error)                                                            \
			throw Exception{                                                  \
				etc::to_string(                                               \
					"FT_" #name,                                              \
					"(" + etc::to_string(__VA_ARGS__) + ")",                  \
					"has error:", error                                       \
				)                                                             \
			};                                                                \
	} while (false)                                                           \
/**/

		struct Face
		{
			::FT_Face handle;

			Face(Infos const& infos, etc::size_type size)
				: handle(nullptr)
			{
				ETC_TRACE.debug("New FreeType Face", infos);
				FT_CALL(
					New_Face,
					freetype::Library::instance().handle,
					infos.path.c_str(),
					infos.face_index,
					&this->handle
				);

				ETC_LOG.debug("Loaded font face",
				              this->handle->family_name,
				              this->handle->style_name);
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
			::FT_BitmapGlyph    bitmap_glyph;
			::FT_Bitmap         bitmap;
			uint32_t            id;
			vector::Vector2f    position;
			vector::Vector2f    size;
			vector::Vector2f    offset;
			vector::Vector2f    advance;

			Glyph(Face& face, uint32_t charcode, uint32_t id)
				: handle(nullptr)
				, bitmap()
				, id(id)
			{
				::FT_GlyphSlot slot = face.handle->glyph;
				ETC_TRACE.debug("New FreeType Glyph of", charcode);
				//auto index = ::FT_Get_Char_Index(face.handle, charcode);
				//if (index == 0)
				//	throw Exception{
				//		"Couldn't find glyph for charcode " + etc::to_string(charcode)
				//	};
				FT_CALL(Load_Char, face.handle, charcode, FT_LOAD_RENDER);
				FT_CALL(Get_Glyph, face.handle->glyph, &this->handle);
				FT_CALL(
					Glyph_To_Bitmap,
					&this->handle,
					FT_RENDER_MODE_NORMAL,      // render mode
					nullptr,                    // origin == (0, 0)
					1                           // destroy original == true
				);
				this->bitmap_glyph = (::FT_BitmapGlyph) this->handle;
				this->bitmap = this->bitmap_glyph->bitmap;

				this->offset.x = slot->metrics.horiBearingX / 64;
				this->offset.y = slot->metrics.horiBearingY / 64;
				this->advance.x = slot->advance.x / 64;
				this->advance.y = slot->advance.y / 65535.0f;
				FT_BBox bbox;
				::FT_Glyph_Get_CBox(this->handle, FT_GLYPH_BBOX_PIXELS, &bbox);

				this->size.x = bbox.xMax - bbox.xMin;
				this->size.y = bbox.yMax - bbox.yMin;
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
				, _tex_coords{}
				, _vertex_buffer{}
				, _next_id{0}
				, _pen{0, 0}
				, _max_line_height{0}
			{
				ETC_TRACE.debug("New GlyphMap");
			}

			vector::Vector2f const&
			tex_coord(etc::size_type index) const
			{
				return _tex_coords[index];
			}

			renderer::Texture& texture() { return *_texture.get(); }

			Glyph& get_glyph(uint32_t c)
			{
				auto it = _glyphs.find(c);
				if (it != _glyphs.end())
					return *it->second.get();
				else
					return _gen_glyph(c);
			}
		private:

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
				uint8_t buffer[glyph.bitmap.width * glyph.bitmap.rows * 4];
				for (int i = 0; i < glyph.bitmap.width; ++i)
					for (int j = 0; j < glyph.bitmap.rows; ++j)
					{
						etc::size_type idx = i + j * glyph.bitmap.width;
						uint8_t col = glyph.bitmap.buffer[idx];

						buffer[idx * 4 + 0] = col;
						buffer[idx * 4 + 1] = col;
						buffer[idx * 4 + 2] = col;
						buffer[idx * 4 + 3] = 255;
					}
				_texture->set_data(_pen.x, _pen.y,
				                   glyph.bitmap.width, glyph.bitmap.rows,
				                   renderer::PixelFormat::rgba,
				                   renderer::ContentPacking::uint8,
				                   buffer);
				_pen.x += glyph.bitmap.width;
				_pen.y += 0; //glyph.bitmap.rows;

				return glyph;
			}
		};

	}} //!anonymous::freetype

	///////////////////////////////////////////////////////////////////////////
	// Font implem class

	struct Font::Impl
	{
	public:
		renderer::Renderer&         renderer;
	private:
		freetype::Face              _face;
		freetype::GlyphMap          _glyphs;

	public:
		Impl(renderer::Renderer& renderer,
		     Infos const& infos,
		     unsigned int size)
			: renderer(renderer)
			, _face{infos, size}
			, _glyphs{_face, renderer}
		{}

		vector::Vector2f const&
		tex_coord(etc::size_type idx) const
		{
			return _glyphs.tex_coord(idx);
		}


		template<typename CharType>
		freetype::Glyph& get_glyph(CharType c)
		{
			return _glyphs.get_glyph(c);
		}

		void bind(renderer::Painter& painter,
		          renderer::ShaderProgramParameter& param)
		{
			param = _glyphs.texture();
			painter.bind(_glyphs.texture());
		}

		void unbind(renderer::Painter& painter)
		{
			painter.unbind(_glyphs.texture());
		}
	};

	///////////////////////////////////////////////////////////////////////////
	// Font class

	Font::Font(renderer::Renderer& renderer,
	           Infos const& infos,
	           etc::size_type size)
		: _impl{new Impl{renderer, infos, size}}
	{}

	Font::~Font()
	{}

	template<typename CharType>
	std::unique_ptr<renderer::VertexBuffer>
	Font::generate_text(std::basic_string<CharType> const& str)
	{
		std::vector<vector::Vector2f>   vertices(str.size() * 4);
		std::vector<vector::Vector2f>   tex_coords(str.size() * 4);

		vector::Vector2f pos{0,0};

		vector::Vector2f max_offset{0, 0};
		for (auto c: str)
		{
			freetype::Glyph& glyph = _impl->get_glyph(c);
			if (glyph.offset.x > max_offset.y)
				max_offset.x = glyph.offset.y;
			if (glyph.offset.y > max_offset.y)
				max_offset.y = glyph.offset.y;
		}
		for (etc::size_type i = 0; i < str.size(); ++i)
		{
			freetype::Glyph& glyph = _impl->get_glyph(str[i]);

			etc::size_type idx0 = i * 4 + 0;
			etc::size_type idx1 = idx0 + 1;
			etc::size_type idx2 = idx0 + 2;
			etc::size_type idx3 = idx0 + 3;


			{
				vector::Vector2f orig(pos.x + glyph.offset.x, pos.y + (max_offset.y - glyph.offset.y));
				vertices[idx0] = orig;
				vertices[idx1] = vector::Vector2f(orig.x + glyph.size.x, orig.y);
				vertices[idx2] = vector::Vector2f(orig.x + glyph.size.x, orig.y + glyph.size.y);
				vertices[idx3] = vector::Vector2f(orig.x, orig.y + glyph.size.y);
			}

			pos.x += glyph.advance.x;
			{
				etc::size_type tex_index = glyph.id * 4;
				tex_coords[idx0] = _impl->tex_coord(tex_index++);
				tex_coords[idx1] = _impl->tex_coord(tex_index++);
				tex_coords[idx2] = _impl->tex_coord(tex_index++);
				tex_coords[idx3] = _impl->tex_coord(tex_index++);
			}
		}

		auto vb = _impl->renderer.new_vertex_buffer(
			renderer::make_vertex_buffer_attribute(
				renderer::ContentKind::vertex,
				&vertices[0],
				vertices.size()
			),
			renderer::make_vertex_buffer_attribute(
				renderer::ContentKind::tex_coord0, // XXX should be configurable
				&tex_coords[0],
				tex_coords.size()
			)
		);
		return vb;
	}

	template
	std::unique_ptr<renderer::VertexBuffer>
	Font::generate_text<char>(std::basic_string<char> const& str);

	template
	std::unique_ptr<renderer::VertexBuffer>
	Font::generate_text<wchar_t>(std::basic_string<wchar_t> const& str);

	template
	std::unique_ptr<renderer::VertexBuffer>
	Font::generate_text<char16_t>(std::basic_string<char16_t> const& str);

	template
	std::unique_ptr<renderer::VertexBuffer>
	Font::generate_text<char32_t>(std::basic_string<char32_t> const& str);

	void Font::bind(renderer::Painter& painter,
	                renderer::ShaderProgramParameter& param)
	{
		_impl->bind(painter, param);
	}

	void Font::unbind(renderer::Painter& painter)
	{
		_impl->unbind(painter);
	}


	Infos::Infos(std::string const& path,
	             std::string const& family_name,
	             std::string const& style_name,
	             Style const style,
	             bool const has_horizontal,
	             bool const has_vertical,
	             bool const is_fixed_width,
	             bool const is_scalable,
	             bool const has_bitmaps,
	             bool const support_kerning,
	             etc::size_type face_index)
		: path{path}
		, family_name{family_name}
		, style_name{style_name}
		, style{style}
		, has_horizontal{has_horizontal}
		, has_vertical{has_vertical}
		, is_fixed_width{is_fixed_width}
		, is_scalable{is_scalable}
		, has_bitmaps{has_bitmaps}
		, support_kerning{support_kerning}
		, face_index{face_index}
	{}

	Infos::~Infos()
	{}

	std::unique_ptr<Font>
	Infos::font(renderer::Renderer& renderer,
	            etc::size_type size)
	{
		return etc::make_unique<Font>(renderer, *this, size);
	}

	etc::size_type
	get_faces_count(std::string const& path) noexcept
	{
		FT_Face face = nullptr;
		auto res = FT_New_Face(
			freetype::Library::instance().handle,
			path.c_str(),
			-1,     // face_index (-1 means only check if the file is loadable).
			&face   // the face to fill (we don't care in this case).
		);
		if (face != nullptr)
		{
			if (res == 0) // 0 means the file can be loaded.
				return face->num_faces;
			FT_Done_Face(face);
		}
		return 0;
	}

	std::unique_ptr<Infos>
	get_face_infos(std::string const& path,
	               etc::size_type face_index)
	{
		FT_Face face = nullptr;
		auto res = FT_New_Face(
			freetype::Library::instance().handle,
			path.c_str(),
			face_index,
			&face  // the face to fill
		);

		// In any case we destroy the face.
		struct RIIA
		{
			FT_Face face;
			~RIIA() { if (face != nullptr) FT_Done_Face(face); }
		} riia{face};

		if (res != 0 || face == nullptr)
			throw Exception{
				etc::to_string("Couldn't open font file:", path)
			};

		Style style;
		switch (face->style_flags)
		{
		case 0:
			style = Style::regular;
			break;
		case FT_STYLE_FLAG_ITALIC:
			style = Style::italic;
			break;
		case FT_STYLE_FLAG_BOLD:
			style = Style::bold;
			break;
		case FT_STYLE_FLAG_BOLD | FT_STYLE_FLAG_ITALIC:
			style = Style::bold_italic;
			break;
		default:
			throw Exception{
				"Unknown style flag value: " + etc::to_string(face->style_flags)
			};
		}

		return std::unique_ptr<Infos>{new Infos{
			path,
			(face->family_name == nullptr ? "" : face->family_name),
			(face->style_name == nullptr ? "" : face->style_name),
			style,
			FT_HAS_HORIZONTAL(face),
			FT_HAS_VERTICAL(face),
			FT_IS_FIXED_WIDTH(face),
			FT_IS_SCALABLE(face),
			FT_HAS_FIXED_SIZES(face),
			FT_HAS_KERNING(face),
			face_index
		}};
	}

	std::list<std::unique_ptr<Infos>>
	get_infos(std::string const& path)
	{
		etc::size_type nb_faces = get_faces_count(path);
		if (nb_faces == 0)
			throw Exception{
				"No face available for font '" + path + "'"
			};

		std::list<std::unique_ptr<Infos>> list;

		for (etc::size_type face_index = 0; face_index < nb_faces; ++face_index)
		{
			list.emplace_back(std::move(get_face_infos(path, face_index)));
		}

		return list;
	}

	std::ostream&
	operator <<(std::ostream& out, Infos const& rhs)
	{
		out << "<Font "
		    << rhs.family_name << ':' << rhs.face_index
		    << " (" << rhs.style;
		if (rhs.has_horizontal)
			out << ", horizontal";
		if (rhs.has_vertical)
			out << ", vertical";
		if (rhs.is_fixed_width)
			out << ", fixed width";
		if (rhs.is_scalable)
			out << ", scalable";
		if (rhs.has_bitmaps)
			out << ", has bitmaps";
		if (rhs.support_kerning)
			out << ", support kerning";
		out << ")"  << " at '" << rhs.path << "'>";
		return out;
	}

	std::ostream&
	operator <<(std::ostream& out, Style const rhs)
	{
		switch (rhs)
		{
		case Style::regular:
			out << "Style::regular";
			break;
		case Style::bold:
			out << "Style::bold";
			break;
		case Style::italic:
			out << "Style::italic";
			break;
		case Style::bold_italic:
			out << "Style::bold_italic";
			break;
		default:
			out << "Style::invalid";
		}
		return out;
	}

}}}
