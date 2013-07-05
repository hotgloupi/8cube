#include "exports.hpp"

#include "../renderer/Shader.hpp"

#include <wrappers/boost/python.hpp>

namespace cube { namespace gl { namespace renderer_bindings {

	namespace py = boost::python;
	using namespace ::cube::gl::renderer;

	void export_shader()
	{
		py::class_<
				Shader,
				std::auto_ptr<Shader>,
				boost::noncopyable
			>(
				"Shader",
				py::no_init
			)
		;
	}

}}}
