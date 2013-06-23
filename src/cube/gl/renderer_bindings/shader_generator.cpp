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
			throw std::runtime_error("Not implemented");
			return true;
		}

		std::string source(ShaderGeneratorProxy const& proxy,
		                   std::string const& name) const override
		{
			throw std::runtime_error("Not implemented");
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
			.def_readonly("type", &ShaderGeneratorProxy::type)
			//.def_readonly("generator", &ShaderGeneratorProxy::generator)
			//.def_readonly("renderer", &ShaderGeneratorProxy::renderer)
			//.def_readonly("parameters", &ShaderGeneratorProxy::parameters)
			//.def_readonly("inputs", &ShaderGeneratorProxy::inputs)
			//.def_readonly("outputs", &ShaderGeneratorProxy::outputs)
			.def("source", &ShaderGeneratorProxy::source)
			.def("shader", &ShaderGeneratorProxy::shader)
			.def("input", &ShaderGeneratorProxy::in, py::return_internal_reference<>())
			.def("output", &ShaderGeneratorProxy::out, py::return_internal_reference<>())
			.def("parameter", &ShaderGeneratorProxy::parameter, py::return_internal_reference<>())
		;

		py::class_<
			BaseShaderRoutine,
			boost::noncopyable
		>("ShaderRoutine")
			.def("source", &ShaderRoutine::source)
			.def("is_applicable", &ShaderRoutine::is_applicable)
		;
	}

}}}
