#include <iostream>
#include <stdexcept>

#include <wrappers/boost/python.hpp>

#include <cube/gl/renderer.hpp>

#include "window.hpp"


namespace detail {

#define MAKE_CONNECTOR_WRAPPER(evt)                                           \
	static boost::signals::connection                                         \
	connect_ ## evt(::cube::system::window::Window& w, boost::python::object& o)         \
	{                                                                         \
		return w.connect_##evt(o);                                            \
	}                                                                         \

	MAKE_CONNECTOR_WRAPPER(expose)
	MAKE_CONNECTOR_WRAPPER(idle)
	MAKE_CONNECTOR_WRAPPER(quit)
	MAKE_CONNECTOR_WRAPPER(resize)

#undef MAKE_CONNECTOR_WRAPPER

	static cube::gl::renderer::detail::WrapRenderer get_renderer(::cube::system::window::Window& w)
	{
		return cube::gl::renderer::detail::WrapRenderer{w.renderer()};
	}

} // !detail


BOOST_PYTHON_MODULE(window)
{
  namespace py = boost::python;

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

	using namespace cube::system::window;

# define DEF_CONNECT(name_)                                             \
		def(                                                                \
			"connect_" # name_,                                               \
			&detail::connect_ ## name_,                                       \
			py::args("on_" #name_ "_callback"),                               \
			"Subscribe to the " #name_ " signal"                              \
		)

	py::class_<Window, boost::noncopyable>("Window",
			"Main window",
			py::init<std::string const&, uint32_t, uint32_t>(
				py::args("title", "width", "height"),
				"Window ctor"
			)
		)
		.DEF_CONNECT(expose)
		.DEF_CONNECT(resize)
		.DEF_CONNECT(idle)
		.DEF_CONNECT(quit)
		.def(
			"poll",
			static_cast<uint32_t (Window::*)()>(&Window::poll)
		)
		.def(
			"poll",
			static_cast<uint32_t (Window::*)(uint32_t)>(&Window::poll)
		)
		.add_property("renderer", &detail::get_renderer)
	;

# undef DEF_CONNECT
}
