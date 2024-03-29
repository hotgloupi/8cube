#include "Node.hpp"
#include "Visitor.hpp"
#include "MultipleVisitor.hpp"
#include "pretty_type.hpp"

#include <cube/scene/Graph.hpp>

#include <cube/exception.hpp>

#include <etc/assert.hpp>
#include <etc/log.hpp>
#include <etc/test.hpp>

#include <iostream>

namespace cube { namespace scene { namespace node {

	using exception::Exception;

	Node::Node(std::string name)
		: _name{std::move(name)}
		, _graph{nullptr}
	{ ETC_TRACE_CTOR(name); }

	Node::~Node()
	{ ETC_TRACE_DTOR(_name); }

	Graph& Node::graph() const
	{
		if (!this->attached())
			throw exception::Exception{"Node is detached"};
		ETC_ASSERT_NEQ(_graph, nullptr);
		return *_graph;
	}

	void Node::attach(Graph& g)
	{
		ETC_LOG.debug("Attach", *this, "to", g);
		ETC_ENFORCE(!this->attached());
		_graph = &g;
	}

	void Node::detach(Graph& g)
	{
		ETC_LOG.debug("Detach", *this, "from", g);
		ETC_ASSERT_EQ(_graph, &g);
		_graph = nullptr;
	}

	Node& Node::_insert_child(std::unique_ptr<Node> node)
	{ return this->graph().insert_child(*this, std::move(node)); }

	void Node::print(std::ostream& out) const ETC_NOEXCEPT
	{
		out << "<" << pretty_type(*this) << " " << _name << ">";
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
			struct Visitor : node::Visitor<Node>
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
			struct Visitor : node::Visitor<Node>
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

		struct Specialized
			: public Node
			, public Visitable<Specialized>
		{
			Specialized()
				: Node{"specialized"}
			{}
			using Visitable<Specialized>::visit;
		};

		struct SpecializedVisitor : node::Visitor<Specialized>
		{
			int& _visited;
			SpecializedVisitor(int& visited) : _visited(visited) {}
			bool visit(Specialized&) override
			{ _visited++; return true; }
		};

		ETC_TEST_CASE(specialized_visitor)
		{

			Graph g;
			int visited = 0;
			SpecializedVisitor v(visited);
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

		struct Specialized1
			: public Node
			, public Visitable<Specialized1>
		{
			Specialized1()
				: Node{"specialized1"}
			{}
			using Visitable<Specialized1>::visit;
		};
		struct Specialized2
			: public Node
			, public Visitable<Specialized2>
		{
			Specialized2()
				: Node{"specialized2"}
			{}
			using Visitable<Specialized2>::visit;
		};
		struct SpecializedMultipleVisitor
			: MultipleVisitor<Specialized1, Specialized2>
		{
			int s1, s2;
			SpecializedMultipleVisitor() : s1{0}, s2{0} {}
			using MultipleVisitor<Specialized1, Specialized2>::visit;
			bool visit(Specialized1&) override
			{ return ++s1; }
			bool visit(Specialized2&) override
			{ return ++s2; }
		};

		ETC_TEST_CASE(multiple_visitor)
		{
			Graph g;
			g.emplace_child<Specialized1>(g.root());
			g.emplace_child<Specialized2>(g.root());
			g.emplace_child<Specialized2>(g.root());

			{
				SpecializedMultipleVisitor v;
				g.traverse(v);
				ETC_ENFORCE_EQ(v.s1, 1);
				ETC_ENFORCE_EQ(v.s2, 2);
			}
		}

	} //!anonymous

}}}
