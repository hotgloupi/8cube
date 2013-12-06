#include "Graph.hpp"

#include <cube/exception.hpp>

#include <etc/log.hpp>
#include <etc/test.hpp>
#include <etc/types.hpp>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

#include <unordered_map>

namespace cube { namespace scene {

	using cube::exception::Exception;

	ETC_LOG_COMPONENT("cube.scene.Graph");

	///////////////////////////////////////////////////////////////////////////
	// Edge, Vertex and RootNode

	namespace {

		struct Edge {};
		struct Vertex { std::shared_ptr<Graph::Node> node; };
		struct RootNode
			: public Graph::Node
		{
			inline explicit
			RootNode(Graph& graph) ETC_NOEXCEPT
				: Node{graph, nullptr}
			{}

			void print(std::ostream& out) const ETC_NOEXCEPT
			{ out << "RootNode"; }
		};

	} // !anonymous

	///////////////////////////////////////////////////////////////////////////
	// Graph::Impl

	struct Graph::Impl
	{
		typedef boost::adjacency_list<
			  boost::vecS
			, boost::vecS
			, boost::directedS
			, Vertex
			, Edge
		> graph_type;

		typedef
			boost::graph_traits<graph_type>::vertex_descriptor
			vertex_descriptor_type;

		static_assert(
			std::is_same<vertex_descriptor_type, Graph::Node::id_type>::value,
			"Graph node id type != boost graph vertex descriptor type"
		);

		graph_type graph;

		std::unordered_map<
			  void const*
			, std::weak_ptr<Graph::Node>
		> node_index;
	};

	///////////////////////////////////////////////////////////////////////////
	// Graph

	Graph::Graph()
		: _this{new Impl}
		, _root(_add(std::unique_ptr<Node>(new RootNode(*this)), nullptr))
	{ ETC_TRACE_CTOR(); }

	Graph::~Graph()
	{
		ETC_TRACE_DTOR();
		// It's important to clear manually here, each node will call
		// Graph::_remove() method in their destructor.
		_this->graph.clear();
	}

	size_t Graph::size() const ETC_NOEXCEPT
	{ return _this->node_index.size(); }

	Graph::Node& Graph::by_data(void const* data)
	{
		auto it = _this->node_index.find(data);
		if (it == _this->node_index.end())
			throw Exception{"Couldn't find any node holding this data"};
		auto ptr = it->second.lock();
		ETC_ASSERT(ptr != nullptr);
		return *ptr;
	}

	Graph::Node& Graph::by_id(id_type const id)
	{
		auto vertex = _this->graph[id];
		if (vertex.node == nullptr)
			throw Exception{
				"Couldn't find any node with the id " + std::to_string(id)
			};
		return *vertex.node;
	}

	void Graph::dump()
	{
		auto p = boost::vertices(_this->graph);
		for (; p.first != p.second; ++p.first)
			etc::print(
				"Node[" + std::to_string(*p.first) + "] =",
				*_this->graph[*p.first].node
			);
	}

	Graph::Node& Graph::_add(std::unique_ptr<Node> node, Node* parent)
	{
		ETC_TRACE.debug("Insert node data", node->data());
		// Transfer ownership.
		std::shared_ptr<Node> ptr{node.get()};
		node.release();

		// Keep track of nodes.
		if (_this->node_index.emplace(ptr->data(), ptr).second == false)
			throw Exception{"Couldn't manage to insert the node"};

		// Insert in the graph.
		Node::id_type id = boost::add_vertex(Vertex{ptr}, _this->graph);
		if (parent != nullptr)
		{
			try { boost::add_edge(parent->id(), id, _this->graph); }
			catch (...) {
				ETC_LOG.error("Couldn't insert an edge");
				// In case of error, we remove the vertex from the graph.
				boost::remove_vertex(id, _this->graph);
				throw;
			}
		}

		// Set the unique id.
		ptr->id(id);
		ETC_LOG.debug("Inserted", *ptr);
		return *ptr;
	}

	void Graph::_remove(Node const& node) ETC_NOEXCEPT
	{
		ETC_LOG.debug("Remove", node, "from index");
		auto it = _this->node_index.find(node.data());
		if (it == _this->node_index.end())
			ETC_LOG.warn("Cannot remove node", node);
		else
			_this->node_index.erase(it);
	}

	///////////////////////////////////////////////////////////////////////////
	// Graph::Node

	void Graph::Node::print(std::ostream& out) const ETC_NOEXCEPT
	{
		_begin_print(out);
		out << "(destroyed)";
		_end_print(out);
	}

	void Graph::Node::_begin_print(std::ostream& out) const ETC_NOEXCEPT
	{
		out << "<Graph::Node " << _id << ": ";
	}

	void Graph::Node::_end_print(std::ostream& out) const ETC_NOEXCEPT
	{
		out << ">";
	}

	Graph::Node::~Node()
	{ ETC_TRACE_DTOR(); _graph._remove(*this); }

	///////////////////////////////////////////////////////////////////////////
	// unit tests

	namespace {

		struct Dummy
		{
			Dummy() {}
			void print(std::ostream& out) const { out << "Dummy"; }
			Dummy(Dummy const&) = delete;
			Dummy& operator =(Dummy const&) = delete;
			Dummy(Dummy&&) = delete;
			Dummy& operator =(Dummy&&) = delete;
		};

		ETC_TEST_CASE(graph_no_node_copy)
		{
			Graph g;
			Dummy a;
			g.add(a); // Compilation will fail if A is copyied.
		}

		ETC_TEST_CASE(graph_empty)
		{
			Graph g;
			ETC_ENFORCE_EQ(g.size(), 1);
			ETC_ENFORCE_EQ(g.root().id(), 0);
			ETC_ENFORCE_EQ(g.root().data(), nullptr);
		}

		ETC_TEST_CASE(graph_node_inserted)
		{
			Graph g;
			Dummy a;
			auto& node = g.add(a);
			ETC_ENFORCE_NEQ(node.id(), 0);
			ETC_ENFORCE_EQ(node.data(), &a);
			ETC_ENFORCE_EQ(g.size(), 2);

			ETC_ENFORCE_EQ(&g[a], &node);
			ETC_ENFORCE_EQ(&g[node.id()], &node);
		}

	} // !anonymous

}} // !cube::scene
