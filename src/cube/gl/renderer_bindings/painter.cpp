#include <wrappers/boost/python.hpp>

#include "PainterWithProxy.hpp"

#include "../renderer/Drawable.hpp"
#include "../renderer/Painter.hpp"
#include "../renderer/State.hpp"
#include "../renderer/ShaderProgram.hpp"
#include "../renderer/VertexBuffer.hpp"
#include "../renderer/VertexBufferAttribute.hpp"

namespace {

	namespace renderer = cube::gl::renderer;
	namespace renderer_bindings = cube::gl::renderer_bindings;

	struct Wrap
	{
		struct Painter
		{
			static
			void bind(renderer::Painter& self, py::list bindables)
			{
				//self.bind(o);
			}
		};

		///////////////////////////////////////////////////////////////////////
		// PainterWithProxy

		struct PainterWithProxy
		{
			static
			void
			__exit__(renderer_bindings::PainterWithProxy& self,
			         PyObject*,
			         PyObject*,
			         PyObject*)
			{
				self.__exit__();
			}
		};
	};

} // !anonymous

namespace cube { namespace gl { namespace renderer_bindings {

	namespace py = boost::python;
	using namespace ::cube::gl::renderer;

	void export_painter()
	{
#define PAINTER_DRAW1(_P1)                                                    \
		static_cast<void(Painter::*)(Drawable<_P1>&, _P1&)>(&Painter::draw)   \
/**/

		py::class_<Painter, boost::noncopyable>(
				"Painter", py::no_init
			)
			.def(
				"bind",
				&Wrap::Painter::bind,
				py::args("drawables", "A list of drawables"),
				"Bound drawables can be unbound with the `unbind' method but are"
				" automatically unbound when the painter is deleted."
			)
			.def(
				"draw_elements",
				&Painter::draw_elements
			)
			.def(
				"draw",
				(void(Painter::*)(Drawable<>&)) &Painter::draw
			)
			.def(
				"draw",
				PAINTER_DRAW1(ShaderProgramParameter)
			)
			.add_property(
				"state",
				py::make_function(
					&Painter::state,
					py::return_internal_reference<>()
				)
			)
		;

		///////////////////////////////////////////////////////////////////////////
		// PainterWithProxy

		py::class_<PainterWithProxy>(
				"PainterWithProxy", py::no_init
			)
			.def(
				"__enter__",
				&PainterWithProxy::__enter__,
				py::return_internal_reference<1>()
			)
			.def(
				"__exit__",
				&Wrap::PainterWithProxy::__exit__
			)
		;
	}

}}}


