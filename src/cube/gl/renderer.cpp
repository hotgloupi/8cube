#include "renderer.hpp"

#include "renderer/Exception.hpp"
#include "renderer/VertexBuffer.hpp"

#include "opengl/Renderer.hpp"

namespace cube { namespace gl { namespace renderer {

	//////////////////////////////////////////////////////////////////////////
	// free functions

	static
	std::vector<cube::gl::renderer::Renderer*> const&
	all_renderers()
	{
		static ::cube::gl::opengl::GLRenderer opengl_renderer;

		static std::vector<Renderer*> renderers{{
			&opengl_renderer,
		}};
		return renderers;
	}

	std::unique_ptr<Renderer>
	create_renderer(cube::gl::viewport::Viewport const& vp,
	                RendererType::Name name)
	{
		for (auto const& renderer: all_renderers())
		{
			if (renderer->description().name() == name)
				return renderer->description().create(vp);
		}
		throw Exception{"Cannot find any renderer with that name"};
	}

}}} // !cube::gl::renderer
