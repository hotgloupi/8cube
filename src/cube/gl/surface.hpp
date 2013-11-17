#ifndef  CUBE_GL_SURFACE_HPP
# define CUBE_GL_SURFACE_HPP

# include "fwd.hpp"
# include "renderer/constants.hpp"

# include <etc/types.hpp>

# include <memory>
# include <string>

namespace cube { namespace gl { namespace surface {

	using renderer::PixelFormat;
	using renderer::ContentPacking;

	class Surface
	{
	private:
		struct Impl;
		std::unique_ptr<Impl> _this;

	public:
		/**
		 * @brief   Create a surface from a file.
		 */
		Surface(std::string const& path);

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

		etc::size_type bytes_per_pixel() const noexcept;
		etc::size_type width() const noexcept;
		etc::size_type height() const noexcept;
		void const* pixels() const noexcept;
	};

}}}

#endif
