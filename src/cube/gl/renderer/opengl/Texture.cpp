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
		int mode = 0;
		if (bpp == 3)
			mode = GL_RGB;
		else if (bpp == 4)
			mode = GL_RGBA;
		else
			assert(false && "Unreachable.");

		//gl::TexImage2D(
		//	GL_TEXTURE_2D,
		//	0,                  // level
		//	bpp,                // 1, 2, 3, 4
		//	_surface->w,
		//	_surface->h,
		//	0,                  // border
		//	mode,               // storage format
		//	GL_UNSIGNED_BYTE,   // component size
		//	_surface->pixels
		//);

		//gluBuild2DMipmaps(
		//	GL_TEXTURE_2D,
		//	bpp,
		//	_surface->w,
		//	_surface->h,
		//	mode,
		//	GL_UNSIGNED_BYTE,
		//	_surface->pixels
		//);

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

#if 0 //ndef __APPLE__
		if (GLEW_VERSION_3_0)
		{
			ETC_LOG.debug(levels, mode == GL_RGB ? "RGB" : "RGBA", surface.width(), surface.height());
			gl::TexStorage2D(GL_TEXTURE_2D, levels, mode, surface.width(), surface.height());
			gl::TexSubImage2D(
				GL_TEXTURE_2D,
				0, 0, 0,
				surface.width(),
				surface.height(),
				GL_BGRA,
				GL_UNSIGNED_BYTE,
				surface.pixels()
			);
		}
		else
#endif // __APPLE__
		{
			gl::TexImage2D(
				GL_TEXTURE_2D,
				0,
				mode,
				surface.width(),
				surface.height(),
				0,
				bpp == 3 ? GL_BGR : GL_BGRA,
				GL_UNSIGNED_BYTE,
				surface.pixels()
			);
		}
		//gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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
		Guard guard(*this);
		gl::ActiveTexture(GL_TEXTURE0 + unit);
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
			data_format, data_packing
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

}}}}
