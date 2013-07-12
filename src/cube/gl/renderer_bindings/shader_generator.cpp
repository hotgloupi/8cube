#include <cube/api.hpp>
#include <cube/gl/renderer/ShaderGenerator.hpp>
#include <cube/gl/renderer/Shader.hpp>

#include <wrappers/boost/python.hpp>

namespace cube { namespace gl { namespace renderer_bindings {
	namespace py = boost::python;
	using namespace ::cube::gl::renderer;

	CUBE_API
	void export_shader_generator()
	{
		py::class_<
			ShaderGenerator,
			std::auto_ptr<ShaderGenerator>,
			boost::noncopyable
		>("ShaderGenerator", py::no_init)
		;

		typedef
			ShaderGeneratorProxy&
			(ShaderGeneratorProxy::*set_param_t)(ShaderParameterType const,
			                                     std::string const&);

		typedef
			ShaderGeneratorProxy&
			(ShaderGeneratorProxy::*set_param_kind_t)(ShaderParameterType const,
			                                          std::string const&,
			                                          ContentKind const);

		py::class_<
			ShaderGeneratorProxy
		>("ShaderGeneratorProxy", py::no_init)
			.def_readonly("type", &ShaderGeneratorProxy::type)
			.def_readonly("parameters", &ShaderGeneratorProxy::parameters)
			.def_readonly("inputs", &ShaderGeneratorProxy::inputs)
			.def_readonly("outputs", &ShaderGeneratorProxy::outputs)
			.def("source", &ShaderGeneratorProxy::source)
			.def("shader", &ShaderGeneratorProxy::shader)
			.def(
				"input",
				static_cast<set_param_t>(&ShaderGeneratorProxy::input),
				py::return_internal_reference<>()
			)
			.def(
				"input",
				static_cast<set_param_kind_t>(&ShaderGeneratorProxy::input),
				py::return_internal_reference<>()
			)
			.def(
				"output",
				static_cast<set_param_t>(&ShaderGeneratorProxy::output),
				py::return_internal_reference<>()
			)
			.def(
				"output",
				static_cast<set_param_kind_t>(&ShaderGeneratorProxy::output),
				py::return_internal_reference<>()
			)
			.def(
				"parameter",
				static_cast<set_param_t>(&ShaderGeneratorProxy::parameter),
				py::return_internal_reference<>()
			)
		;


		struct ShaderRoutineWrap
			: public ShaderRoutine
			, public py::wrapper<ShaderRoutine>
		{
			std::string source(ShaderGeneratorProxy const& proxy,
		                       std::string const& name) const override
			{
				return this->get_override("source")(proxy, name);
			}

			bool is_applicable(ShaderType type) const override
			{
				return this->get_override("is_applicable")(type);
			}
		};

		py::class_<
			ShaderRoutineWrap,
			boost::noncopyable
		>("ShaderRoutine")
			.def("is_applicable", py::pure_virtual(&ShaderRoutineWrap::is_applicable))
			.def("source", py::pure_virtual(&ShaderRoutineWrap::source))
		;
	}

}}}
