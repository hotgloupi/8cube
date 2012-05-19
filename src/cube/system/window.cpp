#include <iostream>
#include <stdexcept>

#include <cube/gl/renderer.hpp>

#include "window.hpp"

# include <wrappers/boost/python.hpp>
namespace py = boost::python;

namespace detail {

#define MAKE_CONNECTOR_WRAPPER(evt)                                           \
	static boost::signals::connection                                         \
	connect_##evt(::cube::system::Window& w, py::object& o)                   \
	{                                                                         \
		return w.connect_##evt(o);                                            \
	}                                                                         \

	MAKE_CONNECTOR_WRAPPER(idle)
	MAKE_CONNECTOR_WRAPPER(expose)

#undef MAKE_CONNECTOR_WRAPPER

} // !detail


BOOST_PYTHON_MODULE(window)
{

	py::class_<boost::signals::connection>(
			"Connection",
			"Window's events slot"
		).def(
			"disconnect",
			&boost::signals::connection::disconnect,
			"Disconnects the signal"
		).def(
			"connected",
			&boost::signals::connection::connected,
			"True if the connection is both initialized and active"
		)
	;

	using namespace cube::system;

	py::class_<Window, boost::noncopyable>("Window",
			"Main window",
			py::init<std::string const&, uint32_t, uint32_t>(
				py::args("title", "width", "height"),
				"Window ctor"
			)
		).def(
			"connect_expose",
			&detail::connect_expose,
			py::args("subscribe_callback"),
			"Subscribe to the expose signal"
		).def(
			"connect_idle",
			&detail::connect_idle
		).def(
			"poll",
			static_cast<uint32_t (Window::*)()>(&Window::poll)
		).def(
			"poll",
			static_cast<uint32_t (Window::*)(uint32_t)>(&Window::poll)
		).def(
			"renderer",
			&Window::renderer,
			py::return_internal_reference<1>()
		)
	;

}
