#include <wrappers/boost/python.hpp>

#include "../renderer/VertexBuffer.hpp"
#include "../renderer/VertexBufferAttribute.hpp"

namespace cube { namespace gl { namespace renderer_bindings {

	namespace py = boost::python;
	using namespace ::cube::gl::renderer;

	void export_vertex_buffer()
	{
		py::class_<
				VertexBuffer,
				std::auto_ptr<VertexBuffer>,
				boost::noncopyable,
				py::bases<Bindable>
			>(
				"VertexBuffer",
				py::no_init
			)
		;
	}

}}}
