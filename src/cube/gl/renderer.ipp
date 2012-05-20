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

		struct WrapRenderer
			: Renderer
			, boost::python::wrapper<Renderer>
		{
			RendererType const& description() const
			{
				std::cerr << __PRETTY_FUNCTION__ << "\n";
				return this->get_override("description")();
			}

			void swap_buffers()
			{
				std::cerr << "swap buffers !\n";
				this->get_override("swap_buffers")();
			}
		~WrapRenderer() {
			std::cerr << __PRETTY_FUNCTION__ << "\n";
		}
			virtual void initialize()
		  {
			throw false;
		  }
			virtual void shutdown()
		  {
			throw false;
		  }
		};

		struct WrapRendererType
			: RendererType
			, boost::python::wrapper<RendererType>
		{
			virtual std::unique_ptr<Renderer> create() const
			{
				throw false;
			}
		};

		void pyexport()
		{
			namespace py = boost::python;

			// Export Renderer class
			py::class_<detail::WrapRenderer, boost::noncopyable>(
					"Renderer"
				).def(
					"description",
					py::pure_virtual(&cube::gl::renderer::Renderer::description),
					py::return_internal_reference<1>()
				)
				.def(
					"swap_buffers",
					py::pure_virtual(&cube::gl::renderer::Renderer::swap_buffers)
				)
			;

			// Export RendererType class
			py::class_<detail::WrapRendererType, boost::noncopyable>(
					"RendererType"
				)
			;
		}

	} // !detail

}}} // !cube::gl::renderer

#endif
