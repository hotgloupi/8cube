#include "Node.hpp"
#include "Visitor.hpp"
#include "MultipleVisitor.hpp"

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

	void Node::detach(Graph& g)
	{
		ETC_LOG.debug("Detach", *this, "from", g, "with id", _id);
		ETC_ASSERT_EQ(_graph, &g);
		_graph = nullptr;
		_id = 0;
	}

	Node& Node::_insert_child(std::unique_ptr<Node> node)
	{ return this->graph().insert_child(*this, std::move(node)); }

	static std::string node_type(Node const* node)
	{
		std::string res = ETC_TYPE_STRING(*node);
		res = res.substr(res.find_last_of(":") + 1, std::string::npos);
		res = res.substr(0, res.find_first_of(" *"));
		res = res.substr(0, res.find_last_of(">"));
		return res;
	}

	void Node::print(std::ostream& out) const ETC_NOEXCEPT
	{
		out << "<" << node_type(this) << " " << _name << ">";
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

		struct Visitor : node::Visitor<Specialized>
		{
			int& _visited;
			Visitor(int& visited) : _visited(visited) {}
			bool visit(Specialized&) override
			{ _visited++; return true; }
		};

		ETC_TEST_CASE(specialized_visitor)
		{

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
		struct SpecializedVisitor
			: MultipleVisitor<Specialized1, Specialized2>
		{
			int s1, s2;
			SpecializedVisitor() : s1{0}, s2{0} {}
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
				SpecializedVisitor v;
				g.traverse(v);
				ETC_ENFORCE_EQ(v.s1, 1);
				ETC_ENFORCE_EQ(v.s2, 2);
			}
		}

	} //!anonymous

}}}
