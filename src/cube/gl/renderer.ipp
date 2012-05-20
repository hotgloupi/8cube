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
		std::cerr << __PRETTY_FUNCTION__ << '\n';
	}

	Renderer::~Renderer()
	{
		std::cerr << __PRETTY_FUNCTION__ << '\n';
	}


	namespace  detail {


		WrapRenderer::WrapRenderer(cube::gl::renderer::Renderer* renderer)
			: renderer(renderer)
		{}

		WrapRenderer::WrapRenderer(WrapRenderer&& other)
			: renderer(other.renderer)
		{}

		WrapRenderer::WrapRenderer(WrapRenderer const& other)
			: renderer(other.renderer)
		{}

		WrapRendererType WrapRenderer::description() const
		{
			assert(this->renderer != nullptr);
			return &this->renderer->description();
		}

		void WrapRenderer::swap_buffers()
		{
			std::cerr << "proxy swap buffers !\n";
			assert(this->renderer != nullptr);
			this->renderer->swap_buffers();
		}

		WrapRenderer::~WrapRenderer()
		{
			std::cerr << __PRETTY_FUNCTION__ << "\n";
		}

		WrapRendererType::WrapRendererType(cube::gl::renderer::RendererType const* renderer_type)
			: renderer_type(renderer_type)
		{}

		WrapRendererType::WrapRendererType(WrapRendererType&& other)
			: renderer_type(other.renderer_type)
		{}

		WrapRendererType::WrapRendererType(WrapRendererType const& other)
			: renderer_type(other.renderer_type)
		{}

	} // !detail

}}} // !cube::gl::renderer

#endif
