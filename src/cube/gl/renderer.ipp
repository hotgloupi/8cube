#ifndef  CUBE_GL_RENDERER_IPP
# define CUBE_GL_RENDERER_IPP

# include <stdexcept>

#include "renderers/all.hpp"

#include "renderer.hpp"

namespace cube { namespace gl { namespace renderer {

	RendererType const& default_renderer_type = renderers::all()[0]->description();

	std::unique_ptr<Renderer>
	create_renderer(RendererType const& description)
	{
		return description.create();
	}

	RendererType const& Renderer::description() const
	{
		throw std::runtime_error("Should have been overrided");
	}

}}}

#endif
