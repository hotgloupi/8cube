#include <string>

#include "renderer.hpp"

# include <wrappers/boost/python.hpp>

using namespace cube::gl::renderer;

static void with_exit(detail::PainterWithProxy& self,
                      PyObject*, PyObject*, PyObject*)
{
  self.__exit__();
}

BOOST_PYTHON_MODULE(renderer)
{
	namespace py = boost::python;

	py::enum_<Renderer::Mode>("RendererMode")
		.value("mode_2d", Renderer::Mode::_2d)
		.value("mode_3d", Renderer::Mode::_3d)
		.export_values()
		.value("none", Renderer::Mode::none)
		.value("_2d", Renderer::Mode::_2d)
		.value("_3d", Renderer::Mode::_3d)
	;

	py::class_<Renderer::Painter, boost::noncopyable>(
			"Painter", py::no_init
		)
	;

	py::class_<detail::PainterWithProxy>(
			"PainterWithProxy", py::no_init
		)
		.def(
			"__enter__",
			&detail::PainterWithProxy::__enter__,
			py::return_internal_reference<1>()
		)
		.def(
			"__exit__",
			&with_exit
		)
	;

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
		.def(
			"viewport",
			&detail::WrapRenderer::viewport
		)
		.def(
			"begin",
			&detail::WrapRenderer::begin
		)

	;

	// Export RendererType class
	py::class_<detail::WrapRendererType>(
		"RendererType"
		)
		.def("__str__", &detail::WrapRendererType::__str__)
	;
}
