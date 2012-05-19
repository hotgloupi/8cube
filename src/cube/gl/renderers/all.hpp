#ifndef  CUBE_GL_RENDERERS_ALL_HPP
# define CUBE_GL_RENDERERS_ALL_HPP

# include <vector>

# include "OpenGL.hpp"

namespace cube { namespace gl { namespace renderers {

	std::vector<cube::gl::Renderer*>& all()
	{
		static std::vector<cube::gl::Renderer*> renderers{{
			new OpenGL{},
		}};
		return renderers;
	}

}}}

#endif
