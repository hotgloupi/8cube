#include <wrappers/boost/python.hpp>

#include "../renderer/Bindable.hpp"

namespace cube { namespace gl { namespace renderer_bindings {

	namespace py = boost::python;
	using namespace ::cube::gl::renderer;

	void export_bindable()
	{
#define EXPORT_BINDABLE()                                                     \
		py::class_<                                                           \
			Bindable<>,                                                       \
			boost::noncopyable,                                               \
			py::bases<BindableBase>                                           \
		>("Bindable", py::no_init)                                            \
	/**/

		py::class_<BindableBase, boost::noncopyable>("BindableBase", py::no_init);
		EXPORT_BINDABLE();
	}

}}}
