#include "angle.hpp"

#include <etc/test.hpp>

#include <iostream>

std::ostream& operator<<(std::ostream& out, cube::units::Angle const angle)
{
	return out << "<Angle " << cube::units::rad_value(angle) << " rad>";
}

namespace cube { namespace units { namespace {

	ETC_TEST_CASE(print)
	{
		ETC_TEST_EQ(etc::to_string(rad(0)), "<Angle 0 rad>");
	}

}}}
