#include <wrappers/boost/python.hpp>

#include "angle.hpp"

namespace cube { namespace units {

	void export_angle()
	{
		namespace py = boost::python;

		py::class_<Angle>("Angle")
		;

		py::def("rad", &rad);
		py::def("rad_value", &rad_value<Angle>);

		py::def("deg", &deg);
		py::def("deg_value", &deg_value<Angle>);

	}

}}
