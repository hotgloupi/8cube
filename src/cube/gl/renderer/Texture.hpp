#ifndef  CUBE_GL_RENDERER_TEXTURE_HPP
# define CUBE_GL_RENDERER_TEXTURE_HPP

# include "Drawable.hpp"
# include "fwd.hpp"

# include <etc/types.hpp>

namespace cube { namespace gl { namespace renderer {

	class Texture
		: public Drawable
	{
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
	};

}}}

#endif
