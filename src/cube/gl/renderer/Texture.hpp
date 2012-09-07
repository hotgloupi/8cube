#ifndef  CUBE_GL_RENDERER_TEXTURE_HPP
# define CUBE_GL_RENDERER_TEXTURE_HPP

# include "Drawable.hpp"
# include "fwd.hpp"

namespace cube { namespace gl { namespace renderer {

	class Texture
		: public Drawable
	{
	public:
		virtual
		void
		bind_unit(unsigned int texture_unit,
		          ShaderProgramParameter* shader = nullptr) = 0;
	};

}}}

#endif
