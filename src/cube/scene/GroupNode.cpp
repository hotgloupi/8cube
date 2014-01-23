#include "GroupNode.hpp"
#include "Graph.hpp"

#include <etc/assert.hpp>
#include <etc/test.hpp>

#include <iostream>
#include <utility>

namespace cube { namespace scene {

	GroupNode::GroupNode(Graph& graph, std::string name)
		: Node{graph, std::move(name)}
		, _children{}
	{}

	void GroupNode::print(std::ostream& out) const ETC_NOEXCEPT
	{ out << "<GroupNode " << _name << ">"; }

	void GroupNode::_add(NodePtr child)
	{
		ETC_ASSERT(child != nullptr);
		Node* raw_ptr = child.get();
		_children.emplace(raw_ptr, std::move(child));
	}

	GroupNode& GroupNode::remove_child(Node& child)
	{
		auto it = _children.find(&child);
		ETC_ASSERT(it != _children.end());
		_children.erase(it);
		child._remove_parent(*this);
		return *this;
	}

	bool GroupNode::has_child(Node& child) const ETC_NOEXCEPT
	{ return _children.find(&child) != _children.end(); }

	namespace {

		ETC_TEST_CASE(visit_children)
		{
			Graph g;
			g.root().emplace<Node>("lol");
			g.root().emplace<Node>("lol");
			g.root().emplace<Node>("lol");

			struct Visitor
				: public NodeVisitor<GroupNode>
			{
				int& _visited;
				Visitor(int& visited) : _visited(visited) {}
				bool visit(GroupNode& node) override
				{ _visited++; node.visit_children(*this); return true; }
				bool visit(Node& node) override
				{
					if (!NodeVisitor<GroupNode>::visit(node))
						_visited++;
					return true;
				}
			};

			int visited;
			Visitor v(visited);

			{
				visited = 0;
				g.root().visit(v);
				ETC_ENFORCE_EQ(visited, 4);
			}

			{
				visited = 0;
				static_cast<Node&>(g.root()).visit(v);
				ETC_ENFORCE_EQ(visited, 4);
			}
		}

	}

}}
