#ifndef CUBE_SCENE_NODEVISITOR_HPP
# define CUBE_SCENE_NODEVISITOR_HPP

# include "fwd.hpp"

namespace cube { namespace scene {

	template<typename NodeType>
	class NodeVisitor;

	template<>
	class NodeVisitor<Node>
	{
	public:
		virtual bool visit(Node& node) = 0;
		virtual ~NodeVisitor() {}
	};

	template<typename NodeType>
	class NodeVisitor
		: public virtual NodeVisitor<Node>
	{
	public:
		bool visit(Node& node) override
		{
			if (auto ptr = dynamic_cast<NodeType*>(&node))
				return this->visit(*ptr);
			return false;
		}

		virtual bool visit(NodeType& node) = 0;
	};

	template<typename... NodeTypes>
	class MultipleNodeVisitor;

	template<typename NodeType, typename... NodeTypes>
	class MultipleNodeVisitor<NodeType, NodeTypes...>
		: public NodeVisitor<NodeType>
		, public MultipleNodeVisitor<NodeTypes...>
	{
	public:
		bool visit(Node& node) override
		{
			if (!NodeVisitor<NodeType>::visit(node))
				return MultipleNodeVisitor<NodeTypes...>::visit(node);
			return true;
		}
	};

	template<>
	class MultipleNodeVisitor<>
	{
	public:
		virtual bool visit(Node&)
		{ return false; }
	};

	template<typename NodeType>
	struct VisitableNode;

	template<>
	struct VisitableNode<Node>
	{
		void visit(NodeVisitor<Node>& visitor);
	};

	template<typename NodeType>
	struct VisitableNode
	{
		void visit(NodeVisitor<Node>& visitor)
		{
			if (auto visitor_ptr = dynamic_cast<NodeVisitor<NodeType>*>(&visitor))
				visitor_ptr->visit(static_cast<NodeType&>(*this));
			else
				visitor.visit(
					static_cast<Node&>(static_cast<NodeType&>(*this))
				);
		}

		void visit(NodeVisitor<NodeType>& visitor)
		{ visitor.visit(static_cast<NodeType&>(*this)); }
	};

}}

#endif
