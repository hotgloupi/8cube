#include "Node.hpp"

#include "Graph.hpp"

#include <cube/exception.hpp>

#include <etc/assert.hpp>
#include <etc/log.hpp>
#include <etc/test.hpp>

#include <iostream>

namespace cube { namespace scene {

	using exception::Exception;

	Node::Node(std::string name)
		: _name{std::move(name)}
		, _graph{nullptr}
		, _id{0}
	{ ETC_TRACE_CTOR(name); }

	Node::~Node()
	{ ETC_TRACE_DTOR(_name); }

	Node::id_type Node::id() const
	{
		if (!this->attached())
			throw exception::Exception{"Node is detached"};
		return _id;
	}

	Graph& Node::graph() const
	{
		if (!this->attached())
			throw exception::Exception{"Node is detached"};
		ETC_ASSERT_NEQ(_graph, nullptr);
		return *_graph;
	}

	void Node::attach(Graph& g, id_type const id)
	{
		ETC_LOG.debug("Attach", *this, "to", g, "with id", id);
		ETC_ENFORCE(!this->attached());
		_graph = &g;
		_id = id; // XXX Should ensure that id is valid in the graph.
	}

	Node& Node::_insert(std::unique_ptr<Node> node)
	{ return this->graph().insert_child(*this, std::move(node)); }

	void Node::print(std::ostream& out) const ETC_NOEXCEPT
	{
		out << "<Node " << _name << ">";
	}

	namespace {

		ETC_TEST_CASE(simple)
		{
			Graph g;
			ETC_ENFORCE(g.root().attached());
			ETC_ENFORCE_EQ(g.root().name(), "root");
			auto& child = g.emplace_child<Node>(g.root(), "test");
			ETC_ENFORCE_EQ(child.name(), "test");
			// XXX ETC_ENFORCE(g.root().has_child(child));
		}

		ETC_TEST_CASE(concrete_visitor)
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
				Specialized()
					: Node{"specialized"}
				{}
				using VisitableNode<Specialized>::visit;
			};

			struct Visitor : NodeVisitor<Specialized>
			{
				int& _visited;
				Visitor(int& visited) : _visited(visited) {}
				bool visit(Specialized&) override
				{ _visited++; return true; }
			};

			Graph g;
			int visited = 0;
			Visitor v(visited);
			{
				g.root().visit(v);
				ETC_ENFORCE_EQ(visited, 0);
			}

			{
				visited = 0;
				auto& sp = g.emplace_child<Specialized>(g.root());
				sp.visit(v);
				ETC_ENFORCE_EQ(visited, 1);
			}
		}

		ETC_TEST_CASE(multiple_visitor)
		{
			struct Specialized1
				: public Node
				, public VisitableNode<Specialized1>
			{
				Specialized1()
					: Node{"specialized1"}
				{}
				using VisitableNode<Specialized1>::visit;
			};
			struct Specialized2
				: public Node
				, public VisitableNode<Specialized2>
			{
				Specialized2()
					: Node{"specialized2"}
				{}
				using VisitableNode<Specialized2>::visit;
			};
			struct Visitor
				: MultipleNodeVisitor<Specialized1, Specialized2>
			{
				int s1, s2;
				Visitor() : s1{0}, s2{0} {}
				using MultipleNodeVisitor<Specialized1, Specialized2>::visit;
				bool visit(Specialized1&) override
				{ return ++s1; }
				bool visit(Specialized2&) override
				{ return ++s2; }
			};
			Graph g;
			g.emplace_child<Specialized1>(g.root());
			g.emplace_child<Specialized2>(g.root());
			g.emplace_child<Specialized2>(g.root());

			{
				Visitor v;
				g.traverse(v);
				ETC_ENFORCE_EQ(v.s1, 1);
				ETC_ENFORCE_EQ(v.s2, 2);
			}
		}

	} //!anonymous

}}
