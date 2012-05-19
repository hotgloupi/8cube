
#include "renderer.hpp"

# include <wrappers/boost/python.hpp>

namespace  detail {

	struct WrapRenderer
		: cube::gl::Renderer
		, boost::python::wrapper<cube::gl::Renderer>
	{
		cube::gl::RendererType const& description() const
		{
			return this->get_override("description")();
		}

		void swap_buffers()
		{
			this->get_override("swap_buffers")();
		}
	};

	struct WrapRendererType
		: cube::gl::RendererType
		, boost::python::wrapper<cube::gl::RendererType>
	{
		virtual std::unique_ptr<cube::gl::Renderer> create() const
		{
			throw false;
		}
	};
}

BOOST_PYTHON_MODULE(renderer)
{

	namespace py = boost::python;

	py::class_<detail::WrapRenderer, boost::noncopyable>(
			"Renderer"
		).def(
			"description",
			py::pure_virtual(&cube::gl::Renderer::description),
			py::return_internal_reference<>()
		)
		.def(
			"swap_buffers",
			py::pure_virtual(&cube::gl::Renderer::swap_buffers)
		)
	;
	py::class_<detail::WrapRendererType, boost::noncopyable>(
			"RendererType"
		)
	;
}
