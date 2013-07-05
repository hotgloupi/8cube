#include "exports.hpp"

#include <wrappers/boost/python.hpp>

#include "../renderer/Bindable.hpp"

namespace cube { namespace gl { namespace renderer_bindings {

	namespace py = boost::python;
	using namespace ::cube::gl::renderer;

	void export_bindable()
	{
		py::class_<Bindable, boost::noncopyable>(
			"Bindable",
			py::no_init
		);
	}

}}}
