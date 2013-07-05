#include <cube/api.hpp>
#include <cube/gl/renderer/VertexBuffer.hpp>
#include <cube/gl/renderer/VertexBufferAttribute.hpp>

#include <wrappers/boost/python.hpp>

namespace cube { namespace gl { namespace renderer_bindings {

	namespace py = boost::python;
	using namespace ::cube::gl::renderer;

	CUBE_API
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
