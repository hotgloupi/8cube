#include <wrappers/boost/python.hpp>

#include "../renderer/Drawable.hpp"

namespace cube { namespace gl { namespace renderer_bindings {

	namespace py = boost::python;
	using namespace ::cube::gl::renderer;

	void export_drawable()
	{
#define EXPORT_DRAWABLE(...)                                                  \
		py::class_<                                                           \
			Drawable<__VA_ARGS__>,                                            \
			boost::noncopyable                                                \
		>("Drawable", py::no_init)                                            \
	/**/

		EXPORT_DRAWABLE();
		EXPORT_DRAWABLE(ShaderProgramParameter);
	}

}}}

