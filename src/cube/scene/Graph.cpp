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

	Graph::Graph(Graph&& other)
		: _this{std::move(other._this)}
	{}

	Graph::Graph(std::unique_ptr<Node> node)
		: _this{new Impl}
	{
		ETC_TRACE_CTOR();
		_this->root = &this->insert(std::move(node));
	}

	Graph::Graph(Node& node, node_deleter_type deleter)
		: _this{new Impl}
	{
		ETC_TRACE_CTOR();
		_this->root = &this->insert(node, std::move(deleter));
	}

	Graph::~Graph()
	{ ETC_TRACE_DTOR(); _this.reset(); }

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

	std::shared_ptr<Node> Graph::remove(Node& node)
	{
		ETC_LOG.debug("Remove", node, "from index");

		auto ptr = _this->graph[node.id()];
		ETC_ASSERT_EQ(ptr.get(), &node);
		ETC_ASSERT_EQ(ptr.use_count(), 2);

		auto id = node.id();
		boost::clear_vertex(id, _this->graph);
		boost::remove_vertex(id, _this->graph);
		ETC_ASSERT_EQ(ptr.unique(), true);

		ptr->detach(*this);
		return ptr;
	}

	void Graph::connect(Node& from, Node& to)
	{
		ETC_ASSERT_EQ(_this->graph[from.id()].get(), &from);
		ETC_ASSERT_EQ(_this->graph[to.id()].get(), &to);
		boost::add_edge(from.id(), to.id(), _this->graph);
	}


	std::vector<Node*> Graph::children(Node& node)
	{
		std::vector<Node*> res;
		Impl::out_edge_iterator it, end;
		for (boost::tie(it, end) = boost::out_edges(node.id(), _this->graph);
		     it != end;
		     ++it)
			res.push_back(_this->graph[boost::target(*it, _this->graph)].get());
		return res;
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
