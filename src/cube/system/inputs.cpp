#include "inputs.hpp"

#include <etc/log.hpp>

#include <ostream>

ETC_LOG_COMPONENT("cube.system.inputs.Inputs");

using namespace cube::system::inputs;

Inputs::~Inputs()
{
#define CHECK_SIGNAL(__name)                                                  \
	if (!_on_ ## __name.empty())                                              \
	{                                                                         \
		ETC_LOG.warn(                                                         \
			#__name " has", _on_ ## __name.num_slots(),                       \
			"not disconnected slots!");                                       \
		_on_ ## __name.disconnect_all_slots();                                \
	}                                                                         \
	/**/
	CHECK_SIGNAL(expose);
	CHECK_SIGNAL(resize);
	CHECK_SIGNAL(idle);
	CHECK_SIGNAL(quit);
	CHECK_SIGNAL(keydown);
	CHECK_SIGNAL(keyup);
	CHECK_SIGNAL(mousemove);
}
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


std::ostream&
operator <<(std::ostream& out,
            KeySym const sym)
{
	out << "KeySym(";
	switch (sym)
	{
	case KeySym::unknown:
		out << "unknown"; break;
	//case KeySym::first:
	//	out << "first"; break;
	case KeySym::backspace:
		out << "backspace"; break;
	case KeySym::tab:
		out << "tab"; break;
	case KeySym::clear:
		out << "clear"; break;
	case KeySym::return_:
		out << "return_"; break;
	case KeySym::pause:
		out << "pause"; break;
	case KeySym::escape:
		out << "escape"; break;
	case KeySym::space:
		out << "space"; break;
	case KeySym::exclaim:
		out << "exclaim"; break;
	case KeySym::quotedbl:
		out << "quotedbl"; break;
	case KeySym::hash:
		out << "hash"; break;
	case KeySym::dollar:
		out << "dollar"; break;
	case KeySym::ampersand:
		out << "ampersand"; break;
	case KeySym::quote:
		out << "quote"; break;
	case KeySym::leftparen:
		out << "leftparen"; break;
	case KeySym::rightparen:
		out << "rightparen"; break;
	case KeySym::asterisk:
		out << "asterisk"; break;
	case KeySym::plus:
		out << "plus"; break;
	case KeySym::comma:
		out << "comma"; break;
	case KeySym::minus:
		out << "minus"; break;
	case KeySym::period:
		out << "period"; break;
	case KeySym::slash:
		out << "slash"; break;
	case KeySym::k0:
		out << "k0"; break;
	case KeySym::k1:
		out << "k1"; break;
	case KeySym::k2:
		out << "k2"; break;
	case KeySym::k3:
		out << "k3"; break;
	case KeySym::k4:
		out << "k4"; break;
	case KeySym::k5:
		out << "k5"; break;
	case KeySym::k6:
		out << "k6"; break;
	case KeySym::k7:
		out << "k7"; break;
	case KeySym::k8:
		out << "k8"; break;
	case KeySym::k9:
		out << "k9"; break;
	case KeySym::colon:
		out << "colon"; break;
	case KeySym::semicolon:
		out << "semicolon"; break;
	case KeySym::less:
		out << "less"; break;
	case KeySym::equals:
		out << "equals"; break;
	case KeySym::greater:
		out << "greater"; break;
	case KeySym::question:
		out << "question"; break;
	case KeySym::at:
		out << "at"; break;
	case KeySym::leftbracket:
		out << "leftbracket"; break;
	case KeySym::backslash:
		out << "backslash"; break;
	case KeySym::rightbracket:
		out << "rightbracket"; break;
	case KeySym::caret:
		out << "caret"; break;
	case KeySym::underscore:
		out << "underscore"; break;
	case KeySym::backquote:
		out << "backquote"; break;
	case KeySym::a:
		out << "a"; break;
	case KeySym::b:
		out << "b"; break;
	case KeySym::c:
		out << "c"; break;
	case KeySym::d:
		out << "d"; break;
	case KeySym::e:
		out << "e"; break;
	case KeySym::f:
		out << "f"; break;
	case KeySym::g:
		out << "g"; break;
	case KeySym::h:
		out << "h"; break;
	case KeySym::i:
		out << "i"; break;
	case KeySym::j:
		out << "j"; break;
	case KeySym::k:
		out << "k"; break;
	case KeySym::l:
		out << "l"; break;
	case KeySym::m:
		out << "m"; break;
	case KeySym::n:
		out << "n"; break;
	case KeySym::o:
		out << "o"; break;
	case KeySym::p:
		out << "p"; break;
	case KeySym::q:
		out << "q"; break;
	case KeySym::r:
		out << "r"; break;
	case KeySym::s:
		out << "s"; break;
	case KeySym::t:
		out << "t"; break;
	case KeySym::u:
		out << "u"; break;
	case KeySym::v:
		out << "v"; break;
	case KeySym::w:
		out << "w"; break;
	case KeySym::x:
		out << "x"; break;
	case KeySym::y:
		out << "y"; break;
	case KeySym::z:
		out << "z"; break;
	case KeySym::delete_:
		out << "delete_"; break;
	case KeySym::kp0:
		out << "kp0"; break;
	case KeySym::kp1:
		out << "kp1"; break;
	case KeySym::kp2:
		out << "kp2"; break;
	case KeySym::kp3:
		out << "kp3"; break;
	case KeySym::kp4:
		out << "kp4"; break;
	case KeySym::kp5:
		out << "kp5"; break;
	case KeySym::kp6:
		out << "kp6"; break;
	case KeySym::kp7:
		out << "kp7"; break;
	case KeySym::kp8:
		out << "kp8"; break;
	case KeySym::kp9:
		out << "kp9"; break;
	case KeySym::kp_period:
		out << "kp_period"; break;
	case KeySym::kp_divide:
		out << "kp_divide"; break;
	case KeySym::kp_multiply:
		out << "kp_multiply"; break;
	case KeySym::kp_minus:
		out << "kp_minus"; break;
	case KeySym::kp_plus:
		out << "kp_plus"; break;
	case KeySym::kp_enter:
		out << "kp_enter"; break;
	case KeySym::kp_equals:
		out << "kp_equals"; break;
	case KeySym::up:
		out << "up"; break;
	case KeySym::down:
		out << "down"; break;
	case KeySym::right:
		out << "right"; break;
	case KeySym::left:
		out << "left"; break;
	case KeySym::insert:
		out << "insert"; break;
	case KeySym::home:
		out << "home"; break;
	case KeySym::end:
		out << "end"; break;
	case KeySym::pageup:
		out << "pageup"; break;
	case KeySym::pagedown:
		out << "pagedown"; break;
	case KeySym::f1:
		out << "f1"; break;
	case KeySym::f2:
		out << "f2"; break;
	case KeySym::f3:
		out << "f3"; break;
	case KeySym::f4:
		out << "f4"; break;
	case KeySym::f5:
		out << "f5"; break;
	case KeySym::f6:
		out << "f6"; break;
	case KeySym::f7:
		out << "f7"; break;
	case KeySym::f8:
		out << "f8"; break;
	case KeySym::f9:
		out << "f9"; break;
	case KeySym::f10:
		out << "f10"; break;
	case KeySym::f11:
		out << "f11"; break;
	case KeySym::f12:
		out << "f12"; break;
	case KeySym::f13:
		out << "f13"; break;
	case KeySym::f14:
		out << "f14"; break;
	case KeySym::f15:
		out << "f15"; break;
	case KeySym::numlock:
		out << "numlock"; break;
	case KeySym::capslock:
		out << "capslock"; break;
	case KeySym::scrollock:
		out << "scrollock"; break;
	case KeySym::rshift:
		out << "rshift"; break;
	case KeySym::lshift:
		out << "lshift"; break;
	case KeySym::rctrl:
		out << "rctrl"; break;
	case KeySym::lctrl:
		out << "lctrl"; break;
	case KeySym::ralt:
		out << "ralt"; break;
	case KeySym::lalt:
		out << "lalt"; break;
	case KeySym::rmeta:
		out << "rmeta"; break;
	case KeySym::lmeta:
		out << "lmeta"; break;
	case KeySym::lsuper:
		out << "lsuper"; break;
	case KeySym::rsuper:
		out << "rsuper"; break;
	case KeySym::mode:
		out << "mode"; break;
	case KeySym::compose:
		out << "compose"; break;
	default:
		out << "invalid: " << (int) sym; break;
	}
	return out << ')';
}
