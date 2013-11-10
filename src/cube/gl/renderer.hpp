#ifndef  CUBE_GL_RENDERER_HPP
# define CUBE_GL_RENDERER_HPP

# include "renderer/fwd.hpp"
# include "renderer/constants.hpp"
# include "renderer/Bindable.hpp"
# include "renderer/Exception.hpp"
# include "renderer/Painter.hpp"
# include "renderer/Renderer.hpp"
# include "renderer/VertexBuffer.hpp"

# include <cube/api.hpp>
# include <cube/system/fwd.hpp>

namespace cube { namespace gl { namespace renderer {

	///////////////////////////////////////////////////////////////////////////
	// high level functions

	/**************************************************************************
	 * Create a renderer.
	 */
	CUBE_API
	RendererPtr
	create_renderer(cube::system::window::RendererContext& context,
	                Name const name = Name::OpenGL);

	/**
	 * Create a new shader generator.
	 *
	 * @note You should use this only if you need multiple shader generators.
	 */
	CUBE_API
	ShaderGeneratorPtr
	create_shader_generator(Renderer& renderer);

}}}

#endif
