#ifndef CUBE_SCENE_NODEVISITOR_HPP
# define CUBE_SCENE_NODEVISITOR_HPP

# include "fwd.hpp"

# include <cube/api.hpp>

# include <etc/log.hpp>

namespace cube { namespace scene {

	template<typename NodeType>
	class NodeVisitor;

	template<>
	class NodeVisitor<Node>
	{
		ETC_LOG_COMPONENT("cube.scene.NodeVisitor<Node>");
	public:
		virtual bool visit(Node& node) = 0;
		virtual ~NodeVisitor() {}
	};

	template<typename NodeType>
	class NodeVisitor
		: public virtual NodeVisitor<Node>
	{
		ETC_LOG_COMPONENT("cube.scene.NodeVisitor<" + ETC_TYPE_STRING(NodeType)
		                  + ">");

	public:
		bool visit(Node& node) override
		{
			ETC_TRACE.debug("Visiting", node);
			if (auto ptr = dynamic_cast<NodeType*>(&node))
			{
				ETC_TRACE.debug("Apply specialized visitor for", node);
				return this->visit(*ptr);
			}
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
		ETC_LOG_COMPONENT("cube.scene.MultipleNodeVisitor<" + ETC_TYPE_STRING(NodeType) + ">");
	public:
		bool visit(Node& node) override
		{
			ETC_TRACE.debug("Visiting", node);
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
		ETC_LOG_COMPONENT("cube.scene.VisitableNode<Node>");
		CUBE_API void visit(NodeVisitor<Node>& visitor);
	};

	template<typename NodeType>
	struct VisitableNode
	{
		ETC_LOG_COMPONENT("cube.scene.VisitableNode<"
		                  + ETC_TYPE_STRING(NodeType) + ">");
		void visit(NodeVisitor<Node>& visitor)
		{
			ETC_TRACE.debug("Dispatching", visitor);
			if (auto visitor_ptr
					= dynamic_cast<NodeVisitor<NodeType>*>(&visitor))
				visitor_ptr->visit(static_cast<NodeType&>(*this));
			else
				visitor.visit(
					static_cast<Node&>(static_cast<NodeType&>(*this))
				);
		}

		void visit(NodeVisitor<NodeType>& visitor)
		{
			ETC_TRACE.debug("Dispatching", visitor);
			visitor.visit(static_cast<NodeType&>(*this));
		}
	};

}}

#endif
