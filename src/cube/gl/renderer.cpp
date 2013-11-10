#include "renderer.hpp"

#include "renderer/Exception.hpp"
#include "renderer/VertexBuffer.hpp"

#include "renderer/opengl/Renderer.hpp"
#include "renderer/opengl/ShaderGenerator.hpp"

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

	RendererPtr
	create_renderer(cube::system::window::RendererContext& context,
	                Name const name)
	{
		for (auto const& description: descriptions())
		{
			if (description->name() == name)
				return description->create(context);
		}
		throw Exception{"Cannot find any renderer with that name"};
	}

	ShaderGeneratorPtr
	create_shader_generator(Renderer& renderer)
	{
		switch (renderer.description().name())
		{
		case Name::OpenGL:
			return etc::make_unique<opengl::ShaderGenerator>(renderer);
		case Name::DirectX:
			throw Exception{"DirectX shader generator not implemented !"};
		default:
			throw Exception{"No shader generator for this renderer"};
		}
	}

}}} // !cube::gl::renderer
