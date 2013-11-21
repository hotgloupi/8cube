#include "surface.hpp"
#include "exception.hpp"

#include <etc/log.hpp>

#include <SDL_image.h>

namespace cube { namespace gl { namespace surface {

	using exception::Exception;

	ETC_LOG_COMPONENT("cube.gl.surface.Surface");

	struct Surface::Impl
	{
		SDL_Surface* surface;

		Impl()
			: surface{nullptr}
		{}

		~Impl()
		{
			if (this->surface != nullptr)
				SDL_FreeSurface(this->surface);
		}
	};

	Surface::Surface(std::string const& path)
		: _this{new Impl}
	{
		ETC_TRACE_CTOR("from", path);
		_this->surface = ::IMG_Load(path.c_str());
		if (_this->surface == nullptr)
			throw Exception{"Couldn't load image '" + path + "'"};
	}
	Surface::Surface(PixelFormat const format,
	                 unsigned int width,
	                 unsigned int height,
	                 PixelFormat const data_format,
	                 ContentPacking const data_packing,
	                 void const* data)
		: _this{new Impl}
	{
		if (data == nullptr)
		{
			auto mask = renderer::pixel_mask(format);
			int depth = renderer::pixel_depth(format);
			_this->surface = SDL_CreateRGBSurface(
				0, width, height, depth,
				mask.red, mask.green, mask.blue, mask.alpha
			);
		}
		else
		{
			if (format != data_format)
				throw Exception{"Conversion not supported"};
			int depth = renderer::pixel_depth(data_format);
			auto mask = renderer::pixel_mask(data_format);
			int pitch = depth / 8 * width;
			_this->surface = SDL_CreateRGBSurfaceFrom(
				(void*)data, width, height, depth, pitch,
				mask.red, mask.green, mask.blue, mask.alpha
			);
		}
		if (_this->surface == nullptr)
			throw Exception{
				"Couldn't create the surface: " + std::string(SDL_GetError())
			};
	}

	Surface::~Surface()
	{ ETC_TRACE_DTOR(); }

	etc::size_type Surface::bytes_per_pixel() const noexcept
	{ return _this->surface->format->BytesPerPixel; }

	etc::size_type Surface::width() const noexcept
	{ return _this->surface->w; }

	etc::size_type Surface::height() const noexcept
	{ return _this->surface->h; }

	void const* Surface::pixels() const noexcept
	{ return _this->surface->pixels; }

	static inline
	etc::size_type component_difference(unsigned char c1,
	                                    unsigned char c2) noexcept
	{ return c1 > c2 ? c1 - c2 : c2 - c1; }

	double Surface::difference(Surface const& other) const
	{
		if (this->width() != other.width() || this->height() != other.height())
			throw Exception{"Cannot compare surface of different sizes"};

		if (this->bytes_per_pixel() != other.bytes_per_pixel())
			throw Exception{"Cannot compare surface of different depth"};
		etc::size_type diff = 0;

		etc::size_type size = this->width() *
		                      this->height() *
		                      this->bytes_per_pixel();
		unsigned char* d1 = (unsigned char*)this->pixels();
		unsigned char* d2 = (unsigned char*)other.pixels();
		for (etc::size_type idx = 0; idx < size; idx++ )
			diff += component_difference(d1[idx], d2[idx]);

		return ((double) diff) / ((double) (size * 255));
	}

}}}
