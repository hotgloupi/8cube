#ifndef  CUBE_GL_RENDERER_TEXTURE_HPP
# define CUBE_GL_RENDERER_TEXTURE_HPP

# include "Bindable.hpp"
# include "fwd.hpp"

# include <etc/types.hpp>

namespace cube { namespace gl { namespace renderer {

	class CUBE_API Texture
		: public Bindable
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

	protected:
		void _bind(State const&) override
		{
			this->_bind();
		}

	protected:
		virtual
		void _bind() = 0;
	};

}}}

#endif
