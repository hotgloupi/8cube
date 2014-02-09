#include "Graph.hpp"
#include "GraphImpl.hpp"
#include "Node.hpp"
#include "NodeVisitor.hpp"

#include <cube/exception.hpp>

#include <etc/log.hpp>
#include <etc/test.hpp>
#include <etc/types.hpp>
#include <etc/scope_exit.hpp>

#include <unordered_set>

namespace cube { namespace scene {

	using cube::exception::Exception;

	ETC_LOG_COMPONENT("cube.scene.Graph");

	///////////////////////////////////////////////////////////////////////////
	// Graph

	Graph::Graph()
		: _this{new Impl}
	{
		ETC_TRACE_CTOR();
		_this->root = &this->emplace<Node>("root");
	}

	Graph::~Graph()
	{ ETC_TRACE_DTOR(); }

	Node& Graph::root() ETC_NOEXCEPT
	{ return *_this->root; }

	size_t Graph::size() const ETC_NOEXCEPT
	{ return boost::num_vertices(_this->graph); }

	Node& Graph::_insert(std::unique_ptr<Node> node)
	{
		ETC_ASSERT(node.get() != nullptr);
		ETC_TRACE.debug("Insert node ", *node);

		// Insert in the graph.
		Node::id_type const id = boost::add_vertex(_this->graph);
		auto remove_vertex
			= etc::scope_exit([&] { boost::remove_vertex(id, _this->graph); });
		// Release the pointer and insert it in the graph.
		_this->graph[id] = std::shared_ptr<Node>{
			node.get(),
			[] (Node* n) { if (!n->attached()) delete n; }
		};
		auto& node_ref = *node.release();

		// Set the node id.
		node_ref.attach(*this, id);

		// Every thing went well, release the guard.
		remove_vertex.dismiss();
		return node_ref;
	}


	Node& Graph::insert(Node& node, std::function<void(Node*)> deleter)
	{
		ETC_TRACE.debug("Insert weak node ", node);

		// Insert in the graph.
		Node::id_type const id = boost::add_vertex(_this->graph);
		auto remove_vertex
			= etc::scope_exit([&] { boost::remove_vertex(id, _this->graph); });
		// Release the pointer and insert it in the graph.
		_this->graph[id] = std::shared_ptr<Node>{&node, deleter};

		// Set the node id.
		node.attach(*this, id);

		// Every thing went well, release the guard.
		remove_vertex.dismiss();
		return node;
	}

	std::unique_ptr<Node> Graph::remove(Node& node)
	{
		ETC_LOG.debug("Remove", node, "from index");

		auto ptr = _this->graph[node.id()];
		ETC_ASSERT_EQ(ptr.get(), &node);
		ETC_ASSERT_EQ(ptr.use_count(), 2);

		auto id = node.id();
		boost::clear_vertex(id, _this->graph);
		boost::remove_vertex(id, _this->graph);
		ETC_ASSERT_EQ(ptr.unique(), 2);

		ptr->detach(*this);
		return std::unique_ptr<Node>{ptr.get()};
	}

	void Graph::connect(Node& from, Node& to)
	{
		ETC_ASSERT_EQ(_this->graph[from.id()].get(), &from);
		ETC_ASSERT_EQ(_this->graph[to.id()].get(), &to);
		boost::add_edge(from.id(), to.id(), _this->graph);
	}

	void Graph::traverse(NodeVisitor<Node>& visitor)
	{
		auto it = boost::vertices(_this->graph).first;
		auto end = boost::vertices(_this->graph).second;
		for (; it != end; ++it)
			visitor.visit(*_this->graph[*it]);
	}

	///////////////////////////////////////////////////////////////////////////
	// unit tests

	namespace {

		ETC_TEST_CASE(empty)
		{
			Graph g;
			ETC_ENFORCE_EQ(g.size(), 1u);
			ETC_ENFORCE_EQ(&g.root().graph(), &g);
		}

	} // !anonymous

}} // !cube::scene
