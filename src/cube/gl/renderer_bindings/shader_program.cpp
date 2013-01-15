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
#define make_eq(__bind_method, __type)                                        \
				.def(                                                         \
					"__eq__",                                                 \
					py::__bind_method(                                        \
						static_cast<                                          \
							void                                              \
							(ShaderProgramParameter::*) (__type)              \
						>(&ShaderProgramParameter::operator =)                \
					)                                                         \
				)                                                             \
				.def(                                                         \
					"set",                                                    \
					py::__bind_method(                                        \
						static_cast<                                          \
							void                                              \
							(ShaderProgramParameter::*) (__type)              \
						>(&ShaderProgramParameter::operator =)                \
					)                                                         \
				)                                                             \
		/**/
				make_eq(pure_virtual, matrix_type const&)
				make_eq(pure_virtual, int32_t)
				make_eq(make_function, Texture&)
#undef make_eq
			;
	}

}}}
