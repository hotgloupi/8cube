#ifndef  CUBE_GL_SURFACE_SURFACE_HPP
# define CUBE_GL_SURFACE_SURFACE_HPP

# include <cube/gl/fwd.hpp>
# include <cube/gl/renderer/constants.hpp>
# include <cube/resource/Resource.hpp>

# include <etc/types.hpp>

# include <wrappers/boost/filesystem.hpp>

# include <memory>
# include <string>

namespace cube { namespace gl { namespace surface {

	using renderer::PixelFormat;
	using renderer::ContentPacking;

	class CUBE_API Surface
		: public resource::Resource
	{
	private:
		struct Impl;
		std::unique_ptr<Impl> _this;

	public:
		/**
		 * @brief   Create a surface from a file.
		 */
		explicit
		Surface(boost::filesystem::path const& path);

		/**
		 * @brief Create an empty RGB surface
		 *
		 * @param   width               New surface width.
		 * @param   height              New surface height.
		 */
		inline
		Surface(unsigned int width,
		        unsigned int height)
			: Surface{PixelFormat::rgb, width, height}
		{}

		/**
		 * @brief Create an empty surface
		 *
		 * @param   format              New surface format.
		 * @param   width               New surface width.
		 * @param   height              New surface height.
		 */
		Surface(PixelFormat const format,
		        unsigned int width,
		        unsigned int height);

		/**
		 * @brief   Create a surface from raw data.
		 *
		 * @param   format              New surface format.
		 * @param   width               New surface width.
		 * @param   height              New surface height.
		 * @param   data_format         Pixel format of the @a data.
		 * @param   data_packing        Pixel packing format of the @a data.
		 * @param   data                Pointer to pixels data.
		 *
		 * The pointer @a data might be NULL, in which case the surface is just
		 * allocated
		 */
		Surface(PixelFormat const format,
		        unsigned int width,
		        unsigned int height,
		        PixelFormat const data_format,
		        ContentPacking const data_packing,
		        void const* data);
		~Surface();

		etc::size_type bytes_per_pixel() const ETC_NOEXCEPT;
		etc::size_type width() const ETC_NOEXCEPT;
		etc::size_type height() const ETC_NOEXCEPT;
		void const* pixels() const ETC_NOEXCEPT;
		PixelFormat pixel_format() const ETC_NOEXCEPT;

		double difference(Surface const& other) const;

		void save_bmp(boost::filesystem::path const& p);

		void fill_rect(rectangle::Rectanglei const& rect,
		               color::Color3f const& color);
	};

}}}

#endif
