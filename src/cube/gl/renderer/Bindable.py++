#include <cube/api.hpp>
#include <cube/gl/renderer/Bindable.hpp>

#include <wrappers/boost/python.hpp>

namespace cube { namespace gl { namespace renderer_bindings {

	namespace py = boost::python;
	using namespace ::cube::gl::renderer;

	CUBE_API
	void export_bindable()
	{
		py::class_<Bindable, boost::noncopyable>(
			"Bindable",
			py::no_init
		);
	}

}}}
