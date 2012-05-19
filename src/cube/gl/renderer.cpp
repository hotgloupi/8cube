
#include "renderer.hpp"

# include <wrappers/boost/python.hpp>

namespace  {

	struct WrapRenderer
		: cube::gl::Renderer
		, boost::python::wrapper<cube::gl::Renderer>
	{
		cube::gl::RendererType const& description()
		{
			return this->get_override("description")();
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

	py::class_<WrapRenderer, boost::noncopyable>(
			"Renderer"
		).def(
			"description",
			py::pure_virtual(&WrapRenderer::description),
			py::return_internal_reference<1>()
		)
	;
	py::class_<WrapRendererType, boost::noncopyable>(
			"RendererType"
		)
	;
}
