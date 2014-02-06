#ifndef CUBE_SCENE_GRAPHIMPL_HPP
# define CUBE_SCENE_GRAPHIMPL_HPP

# include "Graph.hpp"
# include "Node.hpp"

# include <boost/graph/adjacency_list.hpp>
# include <boost/graph/graph_traits.hpp>

# include <memory>
# include <type_traits>

namespace cube { namespace scene {

	struct Graph::Impl
	{
		struct NodeDeleter
		{
			bool released;
			NodeDeleter() : released{false} {}
			NodeDeleter(NodeDeleter const&) = default;
			NodeDeleter& operator =(NodeDeleter const&) = default;
			void operator ()(Node* ptr)
			{ if (!this->released) delete ptr; }
		};

		typedef
			boost::adjacency_list<
				  boost::vecS
				, boost::vecS
				, boost::directedS
				, std::shared_ptr<Node>
				//, boost::no_property
			>
			graph_type;

		typedef boost::graph_traits<graph_type> graph_traits;
		typedef graph_traits::vertex_descriptor vertex_descriptor_type;

		static_assert(
			std::is_same<vertex_descriptor_type, Node::id_type>::value,
			"Adapt Node::id_type to match vertex descriptor type"
		);

		graph_type graph;
		Node* root;
	};

}}

#endif
