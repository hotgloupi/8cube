#include "Graph.hpp"
#include "GraphImpl.hpp"
#include "node/Node.hpp"
#include "node/Visitor.hpp"
#include "visit/breadth_first_search.hpp"
#include "visit/depth_first_search.hpp"

#include <cube/exception.hpp>

#include <etc/log.hpp>
#include <etc/test.hpp>
#include <etc/types.hpp>
#include <etc/scope_exit.hpp>

#include <unordered_set>

namespace cube { namespace scene {

	using cube::exception::Exception;
	using node::Node;
	using node::Visitor;

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
	{
		ETC_TRACE_DTOR();
		for (auto& it: _this->nodes)
		{
			node::Node* n = it.first;
			auto id = it.second;
			n->detach(*this);
			_this->vertex_node_deleter_map[id](n);
		}
		_this.reset();
		ETC_LOG.debug(*this, "All nodes free'd");
	}

	Node& Graph::root() ETC_NOEXCEPT
	{ return *_this->root; }

	size_t Graph::size() const ETC_NOEXCEPT
	{ return boost::num_vertices(_this->graph); }

	Node& Graph::_insert(std::unique_ptr<Node> node)
	{
		this->insert(
			*node,
			[] (Node* ptr) { ETC_ASSERT(!ptr->attached()); delete ptr; }
		);
		return *node.release();
	}

	Node& Graph::insert(Node& node, std::function<void(Node*)> deleter)
	{
		ETC_TRACE.debug("Insert node ", node);

		// Insert in the graph.
		auto const id = boost::add_vertex(_this->graph);
		auto remove_vertex = etc::scope_exit([&] {
			boost::remove_vertex(id, _this->graph);
			_this->nodes.erase(&node);
		});

		// Release the pointer and insert it in the graph.

		boost::put(_this->vertex_node_map, id, &node);
		boost::put(_this->vertex_node_deleter_map, id, deleter);
		_this->nodes.insert(std::make_pair(&node, id));

		node.attach(*this);

		// Every thing went well, release the guard.
		remove_vertex.dismiss();

		_this->call_hooks(Event::insert, node);
		return node;
	}

	std::shared_ptr<Node> Graph::remove(Node& node)
	{
		ETC_LOG.debug("Remove node", node, "from the graph");
		auto it = _this->nodes.find(&node);
		if (it == _this->nodes.end())
			throw Exception{
				etc::to_string("The node", node, "does not belong to", *this)
			};

		auto id = it->second;
		auto deleter = boost::get(_this->vertex_node_deleter_map, id);
		boost::clear_vertex(id, _this->graph);
		boost::remove_vertex(id, _this->graph);
		node.detach(*this);
		_this->nodes.erase(it);

		_this->call_hooks(Event::remove, node);
		return std::shared_ptr<Node>{&node, deleter};
	}

	void Graph::connect(Node& from, Node& to)
	{
		ETC_TRACE.debug("Connect", from, "to", to);
		auto from_id = _this->nodes.at(&from);
		auto to_id = _this->nodes.at(&to);
		boost::add_edge(from_id, to_id, _this->graph);
	}

	std::vector<Node*> Graph::children(Node& node)
	{
		std::vector<Node*> res;
		Impl::out_edge_iterator it, end;
		boost::tie(it, end) = boost::out_edges(
			_this->nodes.at(&node),
			_this->graph
		);
		for (; it != end; ++it)
			res.push_back(
				_this->vertex_node_map[boost::target(*it, _this->graph)]
			);
		return res;
	}

	void Graph::traverse(Visitor<Node>& visitor)
	{
		Impl::vertex_iterator it, end;
		boost::tie(it, end) = boost::vertices(_this->graph);
		for (; it != end; ++it)
			visitor.visit(*_this->vertex_node_map[*it]);
	}

	namespace {
		struct SimpleBFSVisitor
			: public visit::DefaultBreadthFirstVisitor
		{
			Visitor<Node>& _wrapped;

			inline explicit
			SimpleBFSVisitor(Visitor<Node>& wrapped)
				: _wrapped(wrapped)
			{}

			inline void examine_vertex(Node& n) { _wrapped.visit(n); }
		};
	} // !anonymous

	void Graph::breadth_first_search(Visitor<Node>& visitor)
	{
		SimpleBFSVisitor wrapper(visitor);
		visit::breadth_first_search(*this, wrapper);
	}

	namespace {
		struct SimpleDFSVisitor
			: public visit::DefaultDepthFirstVisitor
		{
			Visitor<Node>& _wrapped;

			inline explicit
			SimpleDFSVisitor(Visitor<Node>& wrapped)
				: _wrapped(wrapped)
			{}

			inline void discover_vertex(Node& n) { _wrapped.visit(n); }
		};
	} // !anonymous

	void Graph::depth_first_search(Visitor<Node>& visitor)
	{
		SimpleDFSVisitor wrapper(visitor);
		visit::depth_first_search(*this, wrapper);
	}

	Graph::hook_guard Graph::_add_hook(Event const ev, visitor_ptr_type visitor)
	{
		visitor_type* ptr = visitor.get();
		_this->hook_map[ev].emplace_back(std::move(visitor));
		return hook_guard{*this, *ptr, ev};
	}

	void Graph::_remove_hook(Event const ev, visitor_type& visitor)
	{

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

		ETC_TEST_CASE(children)
		{
			Graph g;
			ETC_ENFORCE_EQ(g.children(g.root()).size(), 0u);
			auto& child = g.root().emplace<Node>("test");
			ETC_ENFORCE_EQ(g.children(g.root()).size(), 1u);

			ETC_ENFORCE_EQ(g.children(child).size(), 0u);
			child.emplace<Node>("last born");
			ETC_ENFORCE_EQ(g.children(child).size(), 1u);
			ETC_ENFORCE_EQ(g.children(g.root()).size(), 1u);
		}

	} // !anonymous

}} // !cube::scene
