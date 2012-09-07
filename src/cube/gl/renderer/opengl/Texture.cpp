#include "Texture.hpp"

#include "Exception.hpp"

#include "../ShaderProgram.hpp"

#include <etc/to_string.hpp>

namespace cube { namespace gl { namespace renderer { namespace opengl {

	Texture::Texture(std::string const& path)
		: _surface(nullptr)
		, _id(0)
	{
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

		gluBuild2DMipmaps(
			GL_TEXTURE_2D,
			bpp,
			_surface->w,
			_surface->h,
			mode,
			GL_UNSIGNED_BYTE,
			_surface->pixels
		);
		gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	Texture::Texture(renderer::PixelFormat const internal_format,
	                 unsigned int width,
	                 unsigned int height,
	                 renderer::PixelFormat const data_format,
	                 renderer::ContentPacking const data_packing,
	                 void const* data)
		: _surface(nullptr)
		, _id(0)
	{
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
			_surface->pixels
		);
	}

	Texture::~Texture()
	{
		SDL_FreeSurface(_surface);
		_surface = nullptr;
		gl::DeleteTextures(1, &_id);
		_id = 0;
	}

	void Texture::_bind()
	{
		gl::Enable(GL_TEXTURE_2D);
		gl::BindTexture(GL_TEXTURE_2D, _id);
	}

	void
	Texture::bind_unit(unsigned int texture_unit,
		               renderer::ShaderProgramParameter* param /* = nullptr */)
	{
		if (!this->_bound())
			this->_bind();
		gl::ActiveTexture(GL_TEXTURE0 + texture_unit);

		if (param != nullptr)
			*param = texture_unit;

		if (!this->_bound())
			this->_unbind();
	}

	void Texture::_unbind()
	{
		gl::BindTexture(GL_TEXTURE_2D, 0);
	}

}}}}
