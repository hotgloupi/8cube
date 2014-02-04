#include "NodeVisitor.hpp"
#include "Node.hpp"

#include <etc/test.hpp>

namespace cube { namespace scene {

	void VisitableNode<Node>::visit(NodeVisitor<Node>& visitor)
	{
		ETC_TRACE.debug("Dispatching", visitor);
		visitor.visit(static_cast<Node&>(*this));
	}

}}
