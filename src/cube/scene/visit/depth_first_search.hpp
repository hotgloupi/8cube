#ifndef  CUBE_SCENE_VISIT_DEPTH_FIRST_SEARCH_HPP
# define CUBE_SCENE_VISIT_DEPTH_FIRST_SEARCH_HPP

# include <cube/scene/GraphImpl.hpp>

# include <boost/graph/depth_first_search.hpp>
# include <boost/concept/assert.hpp>

namespace cube { namespace scene { namespace visit {

	namespace detail {

		template<typename Visitor>
		struct DepthFirstSearchVisitorConcept
		{
		};

		template<typename Visitor>
		struct DepthFirstVisitorWrapper
			: public boost::default_dfs_visitor
		{
			typedef boost::default_dfs_visitor super_type;
			Visitor& _visitor;
			Graph& _graph;
			DepthFirstVisitorWrapper(Graph& graph, Visitor& visitor)
				: _visitor(visitor)
				, _graph(graph)
			{}

			DepthFirstVisitorWrapper(DepthFirstVisitorWrapper const& other)
				: super_type(other)
				, _visitor(other._visitor)
				, _graph(other._graph)
			{}
# define _CUBE_SCENE_VISIT_DFS_FORWARD(__name)                                \
			template<typename V, typename G>                                  \
			inline void __name ## _vertex(V vertex, G const&)                 \
			{ _visitor.__name ## _vertex(*_graph.impl().vertex_node_map[vertex]); }\
/**/
			_CUBE_SCENE_VISIT_DFS_FORWARD(discover);
			_CUBE_SCENE_VISIT_DFS_FORWARD(initialize);
			//_CUBE_SCENE_VISIT_DFS_FORWARD(examine);
			_CUBE_SCENE_VISIT_DFS_FORWARD(finish);
# undef _CUBE_SCENE_VISIT_DFS_FORWARD
		};
	}

	struct DefaultDepthFirstVisitor
	{
		void initialize_vertex(node::Node&) {}
		void discover_vertex(node::Node&) {}
		//void examine_vertex(Node&) {}
		void finish_vertex(node::Node&) {}
	};

	template<typename Visitor>
	void depth_first_search(Graph& graph, Visitor&& v)
	{
		BOOST_CONCEPT_ASSERT(( detail::DepthFirstSearchVisitorConcept<Visitor> ));
		auto& graph_impl = graph.impl().graph;
		detail::DepthFirstVisitorWrapper<Visitor> wrapped(graph, v);
		boost::depth_first_search(
			graph_impl,
			boost::visitor(wrapped)
				.root_vertex(graph.impl().nodes.at(&graph.root()))
				.color_map(graph.impl().vertex_color_map)
		);
	}

}}}

#endif
