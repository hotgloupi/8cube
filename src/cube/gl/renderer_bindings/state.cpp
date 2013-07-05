#include "exports.hpp"

#include "../renderer/State.hpp"

#include <wrappers/boost/python.hpp>

namespace cube { namespace gl { namespace renderer_bindings {

	namespace py = boost::python;
	using namespace ::cube::gl::renderer;

	void export_state()
	{
		typedef void(State::*get_matrix_t)(matrix_type const&);

		py::class_<State, boost::noncopyable>("State", py::init<State const&>())

# define EXPORT_STATE_MATRIX(__name)                                          \
			.add_property(                                                    \
				#__name,                                                      \
				py::make_function(                                            \
					static_cast<                                              \
						matrix_type const&(State::*)() const                  \
					>(&State::__name),                                        \
					py::return_internal_reference<>()                         \
				),                                                            \
				static_cast<get_matrix_t>(&State::__name)                     \
			)                                                                 \
	/**/
			EXPORT_STATE_MATRIX(model)
			EXPORT_STATE_MATRIX(view)
			EXPORT_STATE_MATRIX(projection)
			.add_property(
				"mvp",
				py::make_function(
					static_cast<
						matrix_type const&(State::*)() const
					>(&State::mvp),
					py::return_internal_reference<>()
				)
			)
		;
	}

}}}


