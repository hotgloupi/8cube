#include "Node.hpp"

#include "GroupNode.hpp"
#include "Graph.hpp"

#include <cube/exception.hpp>

#include <etc/assert.hpp>
#include <etc/test.hpp>

#include <iostream>

namespace cube { namespace scene {

	using exception::Exception;

	Node::Node(Graph& graph, std::string name)
		: _name{std::move(name)}
		, _graph(graph)
	{
		_graph._node_register(*this);
	}

	Node::~Node()
	{
		_graph._node_unregister(*this);
	}

	void Node::_add_parent(GroupNode& parent)
	{
		_parents.emplace(
			&parent,
			std::static_pointer_cast<GroupNode>(parent.shared_from_this())
		);
	}

	void Node::_remove_parent(GroupNode& parent)
	{
		auto it = _parents.find(&parent);
		ETC_ASSERT(it != _parents.end());
		_parents.erase(it);
	}

	void Node::print(std::ostream& out) const ETC_NOEXCEPT
	{
		out << "<Node " << _name << ">";
	}

	namespace {

		ETC_TEST_CASE(simple)
		{
			Graph g;
			auto& child = g.root().emplace<Node>("test");
			ETC_ENFORCE_EQ(child.name(), "test");
			ETC_ENFORCE(g.root().has_child(child));
		}

		ETC_TEST_CASE(concrete_visitor)
		{
			Graph g;
			struct Visitor : NodeVisitor<GroupNode>
			{
				int& _visited;
				Visitor(int& visited) : _visited(visited) {}
				bool visit(GroupNode&) override
				{ _visited++; return true; }
			};

			int visited;
			Visitor v(visited);

			{
				visited = 0;
				g.root().visit(v);
				ETC_ENFORCE_EQ(visited, 1);
			}

			{
				visited = 0;
				static_cast<Node&>(g.root()).visit(v);
				ETC_ENFORCE_EQ(visited, 1);
			}
		}

		ETC_TEST_CASE(base_visitor)
		{
			Graph g;
			struct Visitor : NodeVisitor<Node>
			{
				int& _visited;
				Visitor(int& visited) : _visited(visited) {}
				bool visit(Node&) override
				{ _visited++; return true; }
			};

			int visited;
			Visitor v(visited);

			{
				visited = 0;
				g.root().visit(v);
				ETC_ENFORCE_EQ(visited, 1);
			}

			{
				visited = 0;
				static_cast<Node&>(g.root()).visit(v);
				ETC_ENFORCE_EQ(visited, 1);
			}
		}

		ETC_TEST_CASE(specialized_visitor)
		{
			struct Specialized
				: public Node
				, public VisitableNode<Specialized>
			{
				Specialized(Graph& g)
					: Node{g, "specialized"}
				{}
				using VisitableNode<Specialized>::visit;
			};

			//struct Visitor : NodeVisitor<Specialized>
			//{
			//	int& _visited;
			//	Visitor(int& visited) : _visited(visited) {}
			//	bool visit(Specialized&) override
			//	{ _visited++; return true; }
			//};

			//Graph g;
			//int visited = 0;
			//Visitor v(visited);
			//{
			//	g.root().visit(v);
			//	ETC_ENFORCE_EQ(visited, 0);
			//}

			//{
			//	visited = 0;
			//	auto& sp = g.root().emplace<Specialized>();
			//	sp.visit(v);
			//	ETC_ENFORCE_EQ(visited, 1);
			//}
		}

		ETC_TEST_CASE(multiple_visitor)
		{
			//struct Specialized1
			//	: public Node
			//	, public VisitableNode<Specialized1>
			//{
			//	Specialized1(Graph& g)
			//		: Node{g, "specialized1"}
			//	{}
			//	using VisitableNode<Specialized1>::visit;
			//};
			//struct Specialized2
			//	: public Node
			//	, public VisitableNode<Specialized2>
			//{
			//	Specialized2(Graph& g)
			//		: Node{g, "specialized2"}
			//	{}
			//	using VisitableNode<Specialized2>::visit;
			//};
			//struct Visitor
			//	: MultipleNodeVisitor<GroupNode, Specialized1, Specialized2>
			//{
			//	int s1, s2;
			//	Visitor() : s1{0}, s2{0} {}
			//	using MultipleNodeVisitor<Specialized1, Specialized2>::visit;
			//	bool visit(Specialized1&) override
			//	{ return ++s1; }
			//	bool visit(Specialized2&) override
			//	{ return ++s2; }
			//	bool visit(GroupNode& n) override
			//	{ n.visit_children(*this); return true; }
			//};
			//Graph g;
			//g.root().emplace<Specialized1>();
			//g.root().emplace<Specialized2>();
			//g.root().emplace<GroupNode>("lol").emplace<Specialized2>();

			//{
			//	Visitor v;
			//	g.root().visit(v);
			//	ETC_ENFORCE_EQ(v.s1, 1);
			//	ETC_ENFORCE_EQ(v.s2, 2);
			//}

			//{
			//	Visitor v;
			//	static_cast<Node&>(g.root()).visit(v);
			//	ETC_ENFORCE_EQ(v.s1, 1);
			//	ETC_ENFORCE_EQ(v.s2, 2);
			//}
		}

	} //!anonymous

}}
