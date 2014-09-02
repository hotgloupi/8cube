#ifndef CUBE_SCENE_GRAPHIMPL_HPP
# define CUBE_SCENE_GRAPHIMPL_HPP

# include "Graph.hpp"
# include "node/Node.hpp"

# include <etc/enum.hpp>
# include <etc/log.hpp>

# include <cube/exception.hpp>

# include <boost/graph/adjacency_list.hpp>
# include <boost/graph/graph_traits.hpp>

# include <memory>
# include <type_traits>
# include <map>

namespace cube { namespace scene {

	struct vertex_list_containerS {}; // tag

}}

namespace boost {

}

namespace cube { namespace scene {

	struct Graph::Impl
	{
		ETC_LOG_COMPONENT("cube.scene.GraphImpl");

		struct vertex_node_t { typedef boost::vertex_property_tag kind; };
		struct vertex_node_deleter_t { typedef boost::vertex_property_tag kind; };

		typedef
			boost::adjacency_list<
				// The selector for the container used to represent the
				// edge-list for each of the vertices.
				  boost::vecS
				// The selector for the container used to represent the
				// vertex-list of the graph.
				, boost::setS
				, boost::directedS
				// for specifying vertex internal property storage.
				, boost::property<boost::vertex_index_t, uint32_t
				, boost::property<boost::vertex_color_t, boost::default_color_type
				, boost::property<vertex_node_t, node::Node*
				, boost::property<vertex_node_deleter_t, std::function<void(Node*)
				>>>>> // end of vertex properties
				// for specifying edge internal property storage.
				, boost::no_property
			>
			graph_type;

		typedef boost::graph_traits<graph_type> graph_traits;
		typedef graph_traits::vertex_descriptor vertex_descriptor_type;
		typedef graph_traits::adjacency_iterator adjacency_iterator;
		typedef graph_traits::out_edge_iterator out_edge_iterator;
		typedef graph_traits::in_edge_iterator in_edge_iterator;
		typedef graph_traits::vertex_iterator vertex_iterator;
		typedef boost::property_map<graph_type, boost::vertex_index_t>::type vertex_index_map_type;
		typedef boost::property_map<graph_type, boost::vertex_color_t>::type vertex_color_map_type;
		typedef boost::property_map<graph_type, vertex_node_t>::type vertex_node_map_type;
		typedef boost::property_map<graph_type, vertex_node_deleter_t>::type vertex_node_deleter_map_type;
		typedef etc::enum_map<Graph::Event, std::vector<Graph::visitor_ptr_type>> hook_map_type;
		std::map<node::Node*, vertex_descriptor_type> nodes;

		graph_type graph;
		Node* root;
		hook_map_type hook_map;
		vertex_index_map_type vertex_index_map;
		vertex_color_map_type vertex_color_map;
		vertex_node_map_type vertex_node_map;
		vertex_node_deleter_map_type vertex_node_deleter_map;

		Impl()
			: graph()
			, root{nullptr}
			, hook_map()
			, vertex_index_map(boost::get(boost::vertex_index_t(), this->graph))
			, vertex_color_map(boost::get(boost::vertex_color_t(), this->graph))
			, vertex_node_map(boost::get(vertex_node_t(), this->graph))
			, vertex_node_deleter_map(boost::get(vertex_node_deleter_t(), this->graph))
		{}

		void call_hooks(Graph::Event const ev, Node& n) ETC_NOEXCEPT
		{
			try {
				for (auto& hook: this->hook_map[ev])
					hook->visit(n);
			} catch (...) {
				ETC_LOG.error("An", ev, "hook got an error:", etc::exception::string());
			}
		}
	};

}}

#endif
