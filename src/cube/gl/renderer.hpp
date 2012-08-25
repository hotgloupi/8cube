#ifndef  CUBE_GL_RENDERER_HPP
# define CUBE_GL_RENDERER_HPP

# include "renderer/constants.hpp"
# include "renderer/Drawable.hpp"
# include "renderer/Exception.hpp"
# include "renderer/Painter.hpp"
# include "renderer/Renderer.hpp"
# include "renderer/State.hpp"
# include "renderer/VertexBuffer.hpp"

namespace cube { namespace gl { namespace renderer {

	///////////////////////////////////////////////////////////////////////////
	// high level functions

	/**************************************************************************
	 * Create a renderer.
	 */
	std::unique_ptr<Renderer>
	create_renderer(cube::gl::viewport::Viewport const& vp,
	                RendererType::Name name = RendererType::OpenGL);

}}}

#endif
