
#include "renderer.hpp"

# include <wrappers/boost/python.hpp>

using namespace cube::gl::renderer;

BOOST_PYTHON_MODULE(renderer)
{
	namespace py = boost::python;

	// Export Renderer class
	py::class_<detail::WrapRenderer>(
			"Renderer"
		)
		.def(
			"description",
			&detail::WrapRenderer::description
		)
		.def(
			"swap_buffers",
			&detail::WrapRenderer::swap_buffers
		)
	;

	// Export RendererType class
	py::class_<detail::WrapRendererType>(
		"RendererType"
		)
		.def("__str__", &detail::WrapRendererType::__str__)
	;
}
