#ifndef  CUBE_SCENE_NODE_VISITABLE_HPP
# define CUBE_SCENE_NODE_VISITABLE_HPP

# include "fwd.hpp"

# include "Visitor.hpp"

namespace cube { namespace scene { namespace node {

	template<>
	struct CUBE_API Visitable<Node>
	{
		ETC_LOG_COMPONENT("cube.scene.Visitable<Node>");
		void visit(Visitor<Node>& visitor);
	};

	template<typename NodeType>
	struct Visitable
	{
		ETC_LOG_COMPONENT("cube.scene.Visitable<"
		                  + ETC_TYPE_STRING(NodeType) + ">");
		void visit(Visitor<Node>& visitor)
		{
			ETC_TRACE.debug("Dispatching", visitor);
			if (auto visitor_ptr
					= dynamic_cast<Visitor<NodeType>*>(&visitor))
				visitor_ptr->visit(static_cast<NodeType&>(*this));
			else
				visitor.visit(
					static_cast<Node&>(static_cast<NodeType&>(*this))
				);
		}

		void visit(Visitor<NodeType>& visitor)
		{
			ETC_TRACE.debug("Dispatching", visitor);
			visitor.visit(static_cast<NodeType&>(*this));
		}
	};

}}}

#endif
