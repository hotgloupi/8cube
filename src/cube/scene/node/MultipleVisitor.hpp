#ifndef  CUBE_SCENE_NODE_MULTIPLEVISITOR_HPP
# define CUBE_SCENE_NODE_MULTIPLEVISITOR_HPP

# include "Node.hpp"

namespace cube { namespace scene { namespace node {

	template<typename NodeType, typename... NodeTypes>
	class MultipleVisitor<NodeType, NodeTypes...>
		: public Visitor<NodeType>
		, public MultipleVisitor<NodeTypes...>
	{
		ETC_LOG_COMPONENT("cube.scene.MultipleVisitor<" + ETC_TYPE_STRING(NodeType) + ">");
	public:
		bool visit(Node& node) override
		{
			ETC_TRACE.debug("Visiting", node);
			if (!Visitor<NodeType>::visit(node))
				return MultipleVisitor<NodeTypes...>::visit(node);
			return true;
		}
	};

	template<>
	class MultipleVisitor<>
	{
	public:
		virtual bool visit(Node&)
		{ return false; }
	};

}}}

#endif
