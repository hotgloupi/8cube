#ifndef  CUBE_GL_RENDERER_IPP
# define CUBE_GL_RENDERER_IPP

# include <iostream>
# include <stdexcept>

# include <wrappers/boost/python.hpp>

# include "renderers/all.hpp"

# include "renderer.hpp"

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

	Renderer::Renderer()
	{
	}

	Renderer::~Renderer()
	{
	}


	namespace  detail {

		std::string WrapRendererType::__str__() const
		{
			return this->renderer_type->__str__();
		}

		WrapRendererType WrapRenderer::description() const
		{
			assert(this->renderer != nullptr);
			return WrapRendererType{&this->renderer->description()};
		}

		void WrapRenderer::swap_buffers()
		{
			assert(this->renderer != nullptr);
			this->renderer->swap_buffers();
		}

	} // !detail

}}} // !cube::gl::renderer

#endif
