#ifndef  CUBE_GL_RENDERERS_ALL_HPP
# define CUBE_GL_RENDERERS_ALL_HPP

# include <vector>

# include "OpenGL.hpp"

namespace cube { namespace gl { namespace renderers {

	std::vector<cube::gl::renderer::Renderer*> const& all()
	{
		static OpenGL opengl_renderer;
    static std::vector<cube::gl::renderer::Renderer*> renderers{{
			&opengl_renderer,
		}};
		return renderers;
	}

}}}

#endif
