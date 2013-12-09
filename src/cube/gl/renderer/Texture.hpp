#ifndef  CUBE_GL_RENDERER_TEXTURE_HPP
# define CUBE_GL_RENDERER_TEXTURE_HPP

# include "Bindable.hpp"
# include "fwd.hpp"

# include <cube/resource/Resource.hpp>

# include <etc/types.hpp>

namespace cube { namespace gl { namespace renderer {

	class CUBE_API Texture
		: public Bindable
		, public resource::Resource
	{
	public:
		etc::size_type const width;
		etc::size_type const height;
	protected:
		Texture(etc::size_type const width,
		        etc::size_type const height)
			: width{width}
			, height{height}
		{}
	public:
		virtual
		void
		bind_unit(etc::size_type unit, ShaderProgramParameter& param) = 0;

		virtual
		void
		set_data(unsigned int x,
		         unsigned int y,
		         unsigned int width,
		         unsigned int height,
		         renderer::PixelFormat const data_format,
		         renderer::ContentPacking const data_packing,
		         void const* data) = 0;

		/// Set up a linear magnifier filtering.
		virtual
		void mag_filter(TextureFilter const filter) = 0;

		/// Set up a linear minifier filtering.
		virtual
		void min_filter(TextureFilter const filter) = 0;

		/// Set up a bilinear minifier filtering.
		virtual
		void min_filter_bilinear(TextureFilter const filter) = 0;

		/// Set up a trilinear minifier filtering.
		virtual
		void min_filter_trilinear(TextureFilter const filter) = 0;

		/// Generate mip maps
		virtual
		void generate_mipmap(etc::size_type const levels) = 0;

		inline
		void generate_mipmap()
		{
			etc::size_type levels = 1,
			               w = this->width,
			               h = this->height;
			while (w > 1 && h > 1)
			{
				levels += 1;
				w /= 2;
				h /= 2;
			}
			this->generate_mipmap(levels);
		}
	};

}}}

#endif
