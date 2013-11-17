#include "Texture.hpp"

#include "Exception.hpp"

#include "../ShaderProgram.hpp"

#include <etc/to_string.hpp>

namespace cube { namespace gl { namespace renderer { namespace opengl {

	ETC_LOG_COMPONENT("cube.gl.renderer.opengl.Texture");

	Texture::Texture(std::string const& path)
		: _surface(nullptr)
		, _id(0)
		, _unit(-1)
	{
		ETC_TRACE_CTOR("from path", path);
		_surface = ::IMG_Load(path.c_str());
		if (_surface == nullptr)
			throw Exception{etc::to_string(
				"Cannot load '" + path + "':",::IMG_GetError()
			)};
		gl::GenTextures(1, &_id);
		gl::BindTexture(GL_TEXTURE_2D, _id);
		int bpp = _surface->format->BytesPerPixel;
		if (bpp != 3 && bpp != 4)
			throw Exception{etc::to_string(
				"Cannot load image with",
 				_surface->format->BytesPerPixel,
				"bytes per pixel"
			)};
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
			etc::size_type w = _surface->w,
			               h = _surface->h;
			while (w > 1 && h > 1)
			{
				levels += 1;
				w /= 2;
				h /= 2;
			}
		}
		// Bug in ATI drivers
		gl::Enable(GL_TEXTURE_2D);

#ifndef __APPLE__
		if (GLEW_VERSION_3_0)
		{
			ETC_LOG.debug(levels, mode == GL_RGB ? "RGB" : "RGBA", _surface->w, _surface->h);
			gl::TexStorage2D(GL_TEXTURE_2D, levels, mode, _surface->w, _surface->h);
			gl::TexSubImage2D(
				GL_TEXTURE_2D,
				0, 0, 0,
				_surface->w,
				_surface->h,
				GL_BGRA,
				GL_UNSIGNED_BYTE,
				_surface->pixels
			);
		}
		else
#endif // __APPLE__
		{
			gl::TexImage2D(
				GL_TEXTURE_2D,
				0,
				mode,
				_surface->w,
				_surface->h,
				0,
				GL_BGRA,
				GL_UNSIGNED_BYTE,
				_surface->pixels
			);
		}
		gl::GenerateMipmap(GL_TEXTURE_2D);
		gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}

	Texture::Texture(renderer::PixelFormat const internal_format,
	                 unsigned int width,
	                 unsigned int height,
	                 renderer::PixelFormat const data_format,
	                 renderer::ContentPacking const data_packing,
	                 void const* data)
		: _surface(nullptr)
		, _id(0)
		, _unit(-1)
	{
		ETC_TRACE_CTOR("from data");
		gl::GenTextures(1, &_id);
		gl::BindTexture(GL_TEXTURE_2D, _id);
		gl::TexImage2D(
			GL_TEXTURE_2D,
			0,                  // level
			gl::get_pixel_format(internal_format),                // 1, 2, 3, 4
			width,
			height,
			0,                  // border
			gl::get_pixel_format(data_format),
			gl::get_content_packing(data_packing),
			data
		);
		gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	Texture::~Texture()
	{
		ETC_TRACE_DTOR();
		SDL_FreeSurface(_surface);
		_surface = nullptr;
		gl::DeleteTextures<gl::no_throw>(1, &_id);
		_id = 0;
	}

	void Texture::_bind()
	{
		ETC_TRACE.debug("Bind texture", _id);
		gl::Enable(GL_TEXTURE_2D);
		gl::BindTexture(GL_TEXTURE_2D, _id);
	}

	void Texture::_unbind() noexcept
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
		param = unit;
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
		ETC_TRACE.debug("Set data of texture", _id);
		Guard guard(*this);
		gl::TexSubImage2D(GL_TEXTURE_2D, 0,
		                  x, y, width, height,
		                  gl::get_pixel_format(data_format),
		                  gl::get_content_packing(data_packing),
		                  data);
	}

}}}}
