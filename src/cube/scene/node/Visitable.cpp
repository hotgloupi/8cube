#include "Visitable.hpp"
#include "Node.hpp"

#include <etc/test.hpp>

namespace cube { namespace scene { namespace node {

	void Visitable<Node>::visit(Visitor<Node>& visitor)
	{
		ETC_TRACE.debug("Dispatching", visitor);
		visitor.visit(static_cast<Node&>(*this));
	}

}}}
