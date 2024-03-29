#include "Texture.hpp"

#include "Exception.hpp"
#include "../ShaderProgram.hpp"

#include <cube/gl/surface.hpp>

#include <etc/assert.hpp>
#include <etc/scope_exit.hpp>
#include <etc/to_string.hpp>

namespace cube { namespace gl { namespace renderer { namespace opengl {

	ETC_LOG_COMPONENT("cube.gl.renderer.opengl.Texture");

	Texture::Texture(surface::Surface const& surface)
		: Super{surface.width(), surface.height()}
		, _id(0)
		, _unit(-1)
		, _has_mipmaps{false}
	{
		ETC_TRACE_CTOR();
		gl::GenTextures(1, &_id);
		auto cleanup = etc::scope_exit([&] {
			gl::DeleteTextures<gl::no_throw>(1, &_id);
		});
		Guard bind_guard(*this);
		int bpp = surface.bytes_per_pixel();
		if (bpp != 3 && bpp != 4)
			throw Exception(
				"Cannot load image with "
 				+ std::to_string(surface.bytes_per_pixel()) +
				" bytes per pixel"
			);

		// calculate mipmap levels
		etc::size_type levels = 1;
		{
			etc::size_type w = surface.width(),
			               h = surface.height();
			while (w > 1 && h > 1)
			{
				levels += 1;
				w /= 2;
				h /= 2;
			}
		}
		// Bug in ATI drivers
		gl::Enable(GL_TEXTURE_2D);

		GLenum type = GL_UNSIGNED_BYTE;
		GLenum format;
		switch(surface.pixel_format())
		{
		case PixelFormat::rgba:
		case PixelFormat::rgba8:
			format = GL_RGBA;
			break;
		case PixelFormat::bgra8:
			format = GL_BGRA;
			break;
		case PixelFormat::argb8:
			format = GL_BGRA;
			type = GL_UNSIGNED_INT_8_8_8_8_REV;
			break;
		case PixelFormat::abgr8:
			format = GL_RGBA;
			type = GL_UNSIGNED_INT_8_8_8_8_REV;
			break;
		case PixelFormat::bgr8:
			format = GL_BGR;
			break;
		case PixelFormat::rgb:
		case PixelFormat::rgb8:
			format = GL_RGB;
			break;
		default:
			throw Exception{
				"Cannot import surface of pixel format '" +
					etc::to_string(surface.pixel_format()) + "'"
			};
		}

		gl::TexImage2D(
			GL_TEXTURE_2D,
			0,
			bpp == 3 ? GL_RGB : GL_RGBA,
			surface.width(),
			surface.height(),
			0,
			format,
			type,
			surface.pixels()
		);

		gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		cleanup.dismiss();
	}

	void Texture::mag_filter(TextureFilter const filter)
	{
		Guard guard(*this);
		switch (filter)
		{
		case TextureFilter::nearest:
			gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			break;
		case TextureFilter::linear:
			gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;
		default:
			throw Exception{"Unknown filter value"};
		}
	}

	void Texture::min_filter(TextureFilter const filter)
	{
		Guard guard(*this);
		switch (filter)
		{
		case TextureFilter::nearest:
			gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			break;
		case TextureFilter::linear:
			gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			break;
		default:
			throw Exception{"Unknown filter value"};
		}
	}

	void Texture::min_filter_bilinear(TextureFilter const filter)
	{
		if (!_has_mipmaps)
			throw Exception{"Cannot set a bilinear filter without mipmap"};
		Guard guard(*this);
		switch (filter)
		{
		case TextureFilter::nearest:
			gl::TexParameteri(
				GL_TEXTURE_2D,
				GL_TEXTURE_MIN_FILTER,
				GL_NEAREST_MIPMAP_NEAREST
			);
			break;
		case TextureFilter::linear:
			gl::TexParameteri(
				GL_TEXTURE_2D,
				GL_TEXTURE_MIN_FILTER,
				GL_LINEAR_MIPMAP_NEAREST
			);
			break;
		default:
			throw Exception{"Unknown filter value"};
		}
	}

	void Texture::min_filter_trilinear(TextureFilter const filter)
	{
		if (!_has_mipmaps)
			throw Exception{"Cannot set a trilinear filter without mipmap"};
		Guard guard(*this);
		switch (filter)
		{
		case TextureFilter::nearest:
			gl::TexParameteri(
				GL_TEXTURE_2D,
				GL_TEXTURE_MIN_FILTER,
				GL_NEAREST_MIPMAP_LINEAR
			);
			break;
		case TextureFilter::linear:
			gl::TexParameteri(
				GL_TEXTURE_2D,
				GL_TEXTURE_MIN_FILTER,
				GL_LINEAR_MIPMAP_LINEAR
			);
			break;
		default:
			throw Exception{"Unknown filter value"};
		}
	}

	void Texture::generate_mipmap(etc::size_type const)
	{
		// XXX levels not used
		Guard guard(*this);
		gl::GenerateMipmap(GL_TEXTURE_2D);
		_has_mipmaps = true;
	}

	//Texture::Texture(renderer::PixelFormat const internal_format,
	//                 unsigned int width,
	//                 unsigned int height,
	//                 renderer::PixelFormat const data_format,
	//                 renderer::ContentPacking const data_packing,
	//                 void const* data)
	//	: _surface(nullptr)
	//	, _id(0)
	//	, _unit(-1)
	//{
	//	ETC_TRACE_CTOR("from data");
	//	gl::GenTextures(1, &_id);
	//	gl::BindTexture(GL_TEXTURE_2D, _id);
	//	gl::TexImage2D(
	//		GL_TEXTURE_2D,
	//		0,                  // level
	//		gl::get_pixel_format(internal_format),                // 1, 2, 3, 4
	//		width,
	//		height,
	//		0,                  // border
	//		gl::get_pixel_format(data_format),
	//		gl::get_content_packing(data_packing),
	//		data
	//	);
	//	gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//	gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//}

	Texture::~Texture()
	{
		ETC_TRACE_DTOR();
		gl::DeleteTextures<gl::no_throw>(1, &_id);
		_id = 0;
	}

	void Texture::_bind()
	{
		ETC_TRACE.debug("Bind texture", _id);
		gl::Enable(GL_TEXTURE_2D);
		gl::BindTexture(GL_TEXTURE_2D, _id);
	}

	void Texture::_unbind() ETC_NOEXCEPT
	{
		gl::BindTexture<gl::no_throw>(GL_TEXTURE_2D, 0);
	}

	void
	Texture::bind_unit(etc::size_type unit,
	                   renderer::ShaderProgramParameter& param)
	{
		ETC_TRACE.debug("Bind texture unit", unit, "of texture", _id);
		gl::ActiveTexture(GL_TEXTURE0 + unit);
		Guard guard(*this);
		param = (int32_t)unit;
	}

	void
	Texture::set_data(unsigned int x,
	                  unsigned int y,
	                  unsigned int width,
	                  unsigned int height,
	                  renderer::PixelFormat const data_format,
	                  renderer::ContentPacking const data_packing,
	                  void const* data)
	{
		ETC_TRACE.debug(
			"Set data of texture", _id,
			"at", x, y, "of size", width, height,
			data_format, '=', gl::get_pixel_format(data_format),
			data_packing,'=', gl::get_content_packing(data_packing)
		);
		if (width == 0 || height == 0)
			return;
		if (width + x > this->width || height + y > this->height)
			throw Exception{"Trying to write outside of the texture"};

		Guard guard(*this);
		gl::TexSubImage2D(GL_TEXTURE_2D, 0,
		                  x, y, width, height,
		                  gl::get_pixel_format(data_format),
		                  gl::get_content_packing(data_packing),
		                  data);
	}

	void Texture::save_bmp(boost::filesystem::path const& p)
	{
		Guard guard(*this);
		static GLenum const tex_kind = GL_TEXTURE_2D;
		int width, height, fmt, red_size, green_size, blue_size, alpha_size;
		glGetTexLevelParameteriv(tex_kind, 0, GL_TEXTURE_WIDTH, &width);
		glGetTexLevelParameteriv(tex_kind, 0, GL_TEXTURE_HEIGHT, &height);
		glGetTexLevelParameteriv(tex_kind, 0, GL_TEXTURE_RED_SIZE, &red_size);
		glGetTexLevelParameteriv(tex_kind, 0, GL_TEXTURE_GREEN_SIZE, &green_size);
		glGetTexLevelParameteriv(tex_kind, 0, GL_TEXTURE_BLUE_SIZE, &blue_size);
		glGetTexLevelParameteriv(tex_kind, 0, GL_TEXTURE_ALPHA_SIZE, &alpha_size);
		glGetTexLevelParameteriv(tex_kind, 0, GL_TEXTURE_INTERNAL_FORMAT, &fmt);
		ETC_LOG.debug("Internal texture size is", width, height);
		ETC_LOG.debug("Internal texture is", gl::to_pixel_format(fmt));
		ETC_LOG.debug("Component sizes:", red_size, green_size, blue_size, alpha_size);
		{
#define GL_GET(k) \
			{ \
				GLint value; \
				glGetIntegerv(k, &value); \
				ETC_LOG.debug(#k " =", value); \
			} \
/**/

			GL_GET(GL_UNPACK_SWAP_BYTES);
			GL_GET(GL_UNPACK_LSB_FIRST);
			GL_GET(GL_UNPACK_ROW_LENGTH);
			GL_GET(GL_UNPACK_IMAGE_HEIGHT);
			GL_GET(GL_UNPACK_SKIP_PIXELS);
			GL_GET(GL_UNPACK_SKIP_ROWS);
			GL_GET(GL_UNPACK_SKIP_IMAGES);
			GL_GET(GL_UNPACK_ALIGNMENT);
#undef GL_GET
		}
		int bpp = 4;
		std::unique_ptr<char[]> pixels{new char[width * height * bpp]};
		memset(pixels.get(), 0, width * height * bpp);
		gl::GetTexImage(
			tex_kind,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			(void*)pixels.get()
		);
		int pitch = bpp * width;
		SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
			pixels.get(),
			width,
			height,
			bpp * 8,
			pitch,
			0, 0, 0, 0
		);
		if (surface == nullptr)
			throw Exception{"CreateRGBSurfaceFrom"};
		ETC_SCOPE_EXIT{ SDL_FreeSurface(surface); };
		if (SDL_SaveBMP(surface, p.string().c_str()) != 0)
			throw Exception("SDL_SaveBMP");
	}

}}}}
