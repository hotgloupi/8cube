#include "inputs.hpp"

#include <ostream>

using namespace cube::system::inputs;

std::ostream&
operator <<(std::ostream& out,
            KeyMod const mod)
{
	out << "KeyMod(";
	bool first = true;
	if (KeyMod::none == mod)
	{ out << (first ? "" : ", ") << "none"; first = false; }
	if (KeyMod::lshift & mod)
	{ out << (first ? "" : ", ") << "lshift"; first = false; }
	if (KeyMod::rshift & mod)
	{ out << (first ? "" : ", ") << "rshift"; first = false; }
	if (KeyMod::lctrl & mod)
	{ out << (first ? "" : ", ") << "lctrl"; first = false; }
	if (KeyMod::rctrl & mod)
	{ out << (first ? "" : ", ") << "rctrl"; first = false; }
	if (KeyMod::lalt & mod)
	{ out << (first ? "" : ", ") << "lalt"; first = false; }
	if (KeyMod::ralt & mod)
	{ out << (first ? "" : ", ") << "ralt"; first = false; }
	if (KeyMod::lmeta & mod)
	{ out << (first ? "" : ", ") << "lmeta"; first = false; }
	if (KeyMod::rmeta & mod)
	{ out << (first ? "" : ", ") << "rmeta"; first = false; }
	if (KeyMod::num & mod)
	{ out << (first ? "" : ", ") << "num"; first = false; }
	if (KeyMod::caps & mod)
	{ out << (first ? "" : ", ") << "caps"; first = false; }
	if (KeyMod::mode & mod)
	{ out << (first ? "" : ", ") << "mode"; first = false; }
	if (KeyMod::ctrl & mod)
	{ out << (first ? "" : ", ") << "ctrl"; first = false; }
	if (KeyMod::shift & mod)
	{ out << (first ? "" : ", ") << "shift"; first = false; }
	if (KeyMod::alt & mod)
	{ out << (first ? "" : ", ") << "alt"; first = false; }
	if (KeyMod::meta & mod)
	{ out << (first ? "" : ", ") << "meta"; first = false; }
	return out << ')';
}


