#include "Surface.hpp"

#include <cube/gl/exception.hpp>
#include <cube/gl/rectangle.hpp>
#include <cube/gl/color.hpp>

#include <etc/log.hpp>

#include <SDL_image.h>

namespace cube { namespace gl { namespace surface {

	using exception::Exception;

	ETC_LOG_COMPONENT("cube.gl.surface.Surface");

	struct Surface::Impl
	{
		SDL_Surface*   surface;
		PixelFormat    pixel_format;
		ContentPacking packing;

		Impl(SDL_Surface* surface)
			: surface{surface}
		{
			switch(this->surface->format->format)
			{
#define CASE(sdl, pf) case sdl: this->pixel_format = pf; break
				CASE(SDL_PIXELFORMAT_RGB332, PixelFormat::r3_g3_b2);
				CASE(SDL_PIXELFORMAT_RGB444, PixelFormat::rgb4);
				CASE(SDL_PIXELFORMAT_RGB555, PixelFormat::rgb5);
				CASE(SDL_PIXELFORMAT_BGR555, PixelFormat::bgr5);
				CASE(SDL_PIXELFORMAT_ARGB4444, PixelFormat::argb4);
				CASE(SDL_PIXELFORMAT_RGBA4444, PixelFormat::rgba4);
				CASE(SDL_PIXELFORMAT_ABGR4444, PixelFormat::abgr4);
				CASE(SDL_PIXELFORMAT_BGRA4444, PixelFormat::bgra4);
				CASE(SDL_PIXELFORMAT_ARGB1555, PixelFormat::a1_rgb5);
				CASE(SDL_PIXELFORMAT_RGBA5551, PixelFormat::rgb5_a1);
				CASE(SDL_PIXELFORMAT_ABGR1555, PixelFormat::a1_bgr5);
				CASE(SDL_PIXELFORMAT_BGRA5551, PixelFormat::bgr5_a1);
				CASE(SDL_PIXELFORMAT_RGB888, PixelFormat::rgb8);
				CASE(SDL_PIXELFORMAT_RGBX8888, PixelFormat::rgb8);
				CASE(SDL_PIXELFORMAT_BGR888, PixelFormat::bgr8);
				CASE(SDL_PIXELFORMAT_RGB24, PixelFormat::rgb8);
				CASE(SDL_PIXELFORMAT_BGR24, PixelFormat::bgr8);
				CASE(SDL_PIXELFORMAT_ARGB8888, PixelFormat::argb8);
				CASE(SDL_PIXELFORMAT_RGBA8888, PixelFormat::rgba8);
				CASE(SDL_PIXELFORMAT_ABGR8888, PixelFormat::abgr8);
				CASE(SDL_PIXELFORMAT_BGRA8888, PixelFormat::bgra8);
				CASE(SDL_PIXELFORMAT_ARGB2101010, PixelFormat::a2_rgb10);
#undef CASE
			default:
				throw Exception{
					std::string("Cannot find a valid pixel format for ") +
					SDL_GetPixelFormatName(this->surface->format->format)
				};
			}
			ETC_LOG.debug("Created surface of format", this->pixel_format);
		}

		~Impl()
		{
			if (this->surface != nullptr)
				SDL_FreeSurface(this->surface);
		}
	};

	Surface::Surface(boost::filesystem::path const& path)
		: _this{nullptr}
	{
		ETC_TRACE_CTOR("from", path);
		auto surface = ::IMG_Load(path.string().c_str());

		if (surface == nullptr)
		{
			char const* err = IMG_GetError();
			throw Exception{
				"Couldn't load image '" + path.string() + "': " +
				(err != nullptr ? err : "Unknown reason")
			};
		}
		_this.reset(new Impl{surface});
	}

	Surface::Surface(PixelFormat const format,
	                 unsigned int width,
	                 unsigned int height)
		: _this{nullptr}
	{
		auto mask = renderer::pixel_mask(format);
		int depth = renderer::pixel_depth(format);
		SDL_Surface* surface = SDL_CreateRGBSurface(
			0, width, height, depth,
			mask.red, mask.green, mask.blue, mask.alpha
		);
		if (surface == nullptr)
			throw Exception{
				"Couldn't create the surface: " + std::string(SDL_GetError())
			};
	}

	Surface::Surface(PixelFormat const format,
	                 unsigned int width,
	                 unsigned int height,
	                 PixelFormat const data_format,
	                 ContentPacking const data_packing,
	                 void const* data)
		: _this{nullptr}
	{
		SDL_Surface* surface = nullptr;
		if (data == nullptr)
		{
			new (this) Surface(format, width, height);
			return;
			//auto mask = renderer::pixel_mask(format);
			//int depth = renderer::pixel_depth(format);
			//surface = SDL_CreateRGBSurface(
			//	0, width, height, depth,
			//	mask.red, mask.green, mask.blue, mask.alpha
			//);
		}
		else
		{
			if (format != data_format)
				throw Exception{"Conversion not supported"};
			int depth = renderer::pixel_depth(data_format);
			auto mask = renderer::pixel_mask(data_format);
			int pitch = depth / 8 * width;
			ETC_LOG.debug("Creating surface from data with width =", width,
			              "height =", height,
			              "depth =", depth,
			              "pitch =", pitch,
			              "mask.red =", mask.red,
			              "mask.green =", mask.green,
			              "mask.blue =", mask.blue,
			              "mask.alpha =", mask.alpha);
			surface = SDL_CreateRGBSurfaceFrom(
				(void*)data, width, height, depth, pitch,
				mask.red, mask.green, mask.blue, mask.alpha
			);
		}
		if (surface == nullptr)
			throw Exception{
				"Couldn't create the surface: " + std::string(SDL_GetError())
			};
		_this.reset(new Impl{surface});
	}

	Surface::~Surface()
	{ ETC_TRACE_DTOR(); }

	etc::size_type Surface::bytes_per_pixel() const ETC_NOEXCEPT
	{ return _this->surface->format->BytesPerPixel; }

	etc::size_type Surface::width() const ETC_NOEXCEPT
	{ return _this->surface->w; }

	etc::size_type Surface::height() const ETC_NOEXCEPT
	{ return _this->surface->h; }

	void const* Surface::pixels() const ETC_NOEXCEPT
	{ return _this->surface->pixels; }

	PixelFormat Surface::pixel_format() const ETC_NOEXCEPT
	{ return _this->pixel_format; }

	static inline
	etc::size_type component_difference(unsigned char c1,
	                                    unsigned char c2) ETC_NOEXCEPT
	{ return c1 > c2 ? c1 - c2 : c2 - c1; }

	double Surface::difference(Surface const& other) const
	{
		if (this->width() != other.width() || this->height() != other.height())
			throw Exception{"Cannot compare surface of different sizes"};

		if (this->bytes_per_pixel() != other.bytes_per_pixel())
			throw Exception{
				"Cannot compare surface of different depth: " +
				etc::to_string(this->bytes_per_pixel()) + " != " +
				etc::to_string(other.bytes_per_pixel())
			};
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

	void Surface::save_bmp(boost::filesystem::path const& p)
	{
		if (SDL_SaveBMP(_this->surface, p.string().c_str()) != 0)
			throw Exception{
				"Couldn't save the surface to '" + p.string() +
				"': " + std::string(SDL_GetError())
			};
	}


	void Surface::fill_rect(rectangle::Rectanglei const& rect, color::Color3f const& color)
	{
		::SDL_Rect r = {rect.x, rect.y, rect.w, rect.h};

		int res = ::SDL_FillRect(
			_this->surface,
			&r,
			SDL_MapRGB(
				_this->surface->format,
				static_cast<uint8_t>(color.r * 255.0f),
				static_cast<uint8_t>(color.g * 255.0f),
				static_cast<uint8_t>(color.b * 255.0f)
			)
		);
		if (res != 0)
			throw Exception{
				etc::to_string(
					"Couldn't fill", rect, "with", color,
					etc::io::nosep(), ": ", SDL_GetError()
				)
			};
	}

}}}
