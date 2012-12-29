#include "renderer.hpp"

#include "renderer/Exception.hpp"
#include "renderer/VertexBuffer.hpp"

#include "renderer/opengl/Renderer.hpp"

#include <etc/memory.hpp>

namespace cube { namespace gl { namespace renderer {

	//////////////////////////////////////////////////////////////////////////
	// free functions

	static
	std::vector<RendererType*> const&
	descriptions()
	{
		static auto opengl_descr = etc::make_unique<opengl::RendererType>();
		static std::vector<RendererType*> renderers{
			opengl_descr.get(),
		};
		return renderers;
	}

	std::unique_ptr<Renderer>
	create_renderer(cube::gl::viewport::Viewport const& vp,
	                RendererType::Name name)
	{
		for (auto const& description: descriptions())
		{
			if (description->name() == name)
				return description->create(vp);
		}
		throw Exception{"Cannot find any renderer with that name"};
	}

}}} // !cube::gl::renderer
