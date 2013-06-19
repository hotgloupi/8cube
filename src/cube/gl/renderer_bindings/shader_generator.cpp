#include <wrappers/boost/python.hpp>

#include "../renderer/ShaderGenerator.hpp"
#include "../renderer/Shader.hpp"

namespace cube { namespace gl { namespace renderer_bindings {
	namespace py = boost::python;
	using namespace ::cube::gl::renderer;

	class BaseShaderRoutine
		: public ShaderRoutine
	{
	public:
		bool is_applicable(ShaderType type) const override
		{
			return true;
		}

		std::string source(ShaderGeneratorProxy const& proxy,
		                   std::string const& name) const override
		{
			return "";
		}
	};

	void export_shader_generator()
	{
		py::class_<
			ShaderGenerator,
			std::auto_ptr<ShaderGenerator>,
			boost::noncopyable
		>("ShaderGenerator", py::no_init)
		;

		py::class_<
			ShaderGeneratorProxy
		>("ShaderGeneratorProxy", py::no_init)
			.def("source", &ShaderGeneratorProxy::source)
			.def("shader", &ShaderGeneratorProxy::shader)
			.def("input", &ShaderGeneratorProxy::in, py::return_internal_reference<>())
			.def("output", &ShaderGeneratorProxy::out, py::return_internal_reference<>())
			.def("parameter", &ShaderGeneratorProxy::shader)
		;

		py::class_<
			BaseShaderRoutine,
			boost::noncopyable
		>("ShaderRoutine")
			.def("source", &ShaderRoutine::source)
		;
	}

}}}
