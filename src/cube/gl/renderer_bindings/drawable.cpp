#include <cube/api.hpp>
#include <cube/gl/renderer/Drawable.hpp>

#include <wrappers/boost/python.hpp>

namespace cube { namespace gl { namespace renderer_bindings {

	namespace py = boost::python;
	using namespace ::cube::gl::renderer;

	CUBE_API
	void export_drawable()
	{
		py::class_<Drawable, boost::noncopyable, std::auto_ptr<Drawable>>(
				"Drawable", py::no_init
			)
		;
	}

}}}
