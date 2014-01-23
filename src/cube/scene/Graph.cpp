#include "Graph.hpp"
#include "GroupNode.hpp"

#include <cube/exception.hpp>

#include <etc/log.hpp>
#include <etc/test.hpp>
#include <etc/types.hpp>

#include <unordered_set>

namespace cube { namespace scene {

	using cube::exception::Exception;

	ETC_LOG_COMPONENT("cube.scene.Graph");

	///////////////////////////////////////////////////////////////////////////
	// Graph::Impl

	struct Graph::Impl
	{
		std::unordered_set<Node*>  nodes;
		std::unique_ptr<GroupNode> root;
	};

	///////////////////////////////////////////////////////////////////////////
	// Graph

	Graph::Graph()
		: _this{new Impl}
	{
		ETC_TRACE_CTOR();
		// Node creation act on the graph, that's why we initialize root node
		// last.
		_this->root.reset(new GroupNode{*this, "root"});
	}

	Graph::~Graph()
	{
		ETC_TRACE_DTOR();
		// Node destruction act on the graph too, we need to do it manually.
		_this->root.reset();
	}

	GroupNode& Graph::root() ETC_NOEXCEPT
	{ return *_this->root; }

	size_t Graph::size() const ETC_NOEXCEPT
	{ return _this->nodes.size(); }

	void Graph::_node_register(Node& node)
	{ _this->nodes.insert(&node); }

	void Graph::_node_unregister(Node& node)
	{ _this->nodes.erase(&node); }

	void Graph::dump()
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

	} // !anonymous

}} // !cube::scene
