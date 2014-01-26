#include "NodeVisitor.hpp"
#include "Node.hpp"

#include <etc/test.hpp>

namespace cube { namespace scene {

	//template
	void VisitableNode<Node>::visit(NodeVisitor<Node>& visitor)
	{ visitor.visit(static_cast<Node&>(*this)); }

	ETC_TEST_CASE(simple)
	{
		struct simple
			: public VisitableNode<simple>
		{};
	}

}}
