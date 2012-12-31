#include <wrappers/boost/python.hpp>

#include "../renderer/ShaderProgram.hpp"
#include "../renderer/State.hpp"
#include "../renderer/Texture.hpp"

namespace cube { namespace gl { namespace renderer_bindings {

	namespace py = boost::python;
	using namespace ::cube::gl::renderer;

	void export_shader_program()
	{
		{
			py::scope scope(py::class_<
					ShaderProgram,
					std::auto_ptr<ShaderProgram>,
					py::bases<Bindable>,
					boost::noncopyable
				>(
					"ShaderProgram",
					py::no_init
				)
				.def(
					"parameter",
					&ShaderProgram::parameter,
					py::return_internal_reference<>()
				)
				.def(
					"update",
					&ShaderProgram::update
				)
			);

		} // end of ShaderProgram scope

		///////////////////////////////////////////////////////////////////////
		// ShaderProgramParameter

			py::class_<
					ShaderProgramParameter,
					boost::noncopyable
				>(
					"ShaderProgramParameter",
					py::no_init
				)
#define __EQ__(type)                                                          \
				.def(                                                         \
					"__eq__",                                                 \
					py::pure_virtual(                                         \
						static_cast<                                          \
							void                                              \
							(ShaderProgramParameter::*) (type)                \
						>(&ShaderProgramParameter::operator =)                \
					)                                                         \
				)                                                             \
		/**/
				__EQ__(matrix_type const&)
				__EQ__(int32_t)
				__EQ__(Texture&)
#undef __EQ__
			;
	}

}}}
