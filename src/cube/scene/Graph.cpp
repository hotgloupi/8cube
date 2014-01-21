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
		GroupNode root;
		std::unordered_set<Node*> nodes;

		Impl(Graph& g)
			: root(g, "root")
		{}
	};

	///////////////////////////////////////////////////////////////////////////
	// Graph

	Graph::Graph()
		: _this{new Impl{*this}}
	{ ETC_TRACE_CTOR(); }

	Graph::~Graph()
	{ ETC_TRACE_DTOR(); }

	GroupNode& Graph::root() ETC_NOEXCEPT
	{ return _this->root; }

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
		}

	} // !anonymous

}} // !cube::scene
