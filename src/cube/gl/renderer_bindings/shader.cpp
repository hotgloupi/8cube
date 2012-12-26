#include "exports.hpp"

#include "../renderer/Shader.hpp"

namespace cube { namespace gl { namespace renderer_bindings {

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
