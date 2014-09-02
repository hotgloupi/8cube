#ifndef  CUBE_SCENE_VISIT_BREADTH_FIRST_SEARCH_HPP
# define CUBE_SCENE_VISIT_BREADTH_FIRST_SEARCH_HPP

# include <cube/scene/GraphImpl.hpp>

# include <boost/graph/breadth_first_search.hpp>
# include <boost/concept/assert.hpp>

namespace cube { namespace scene { namespace visit {

	namespace detail {

		template<typename Visitor>
		struct BreadthFirstVisitorConcept
		{
			void constraints()
			{
				node::Node* node = nullptr;
				_visitor.initialize_vertex(*node);
				_visitor.discover_vertex(*node);
				_visitor.examine_vertex(*node);
				_visitor.finish_vertex(*node);
				//vis.examine_edge(e, g);
				//vis.tree_edge(e, g);
				//vis.non_tree_edge(e, g);
				//vis.gray_target(e, g);
				//vis.black_target(e, g);
			}
			Visitor _visitor;
		};

		template<typename Visitor>
		struct BreadthFirstVisitorWrapper
			: public boost::default_bfs_visitor
		{
			typedef boost::default_bfs_visitor super_type;
			Visitor& _visitor;
			Graph& _graph;
			BreadthFirstVisitorWrapper(Graph& graph, Visitor& visitor)
				: _visitor(visitor)
				, _graph(graph)
			{}
			BreadthFirstVisitorWrapper(BreadthFirstVisitorWrapper const& other)
				: super_type(other)
				, _visitor(other._visitor)
				, _graph(other._graph)
			{}
# define _CUBE_SCENE_VISIT_BFS_FORWARD(__name)                                \
			template<typename V, typename G>                                  \
			inline void __name ## _vertex(V vertex, G const&)                 \
			{ _visitor.__name ## _vertex(*_graph.impl().vertex_node_map[vertex]); }\
/**/
			_CUBE_SCENE_VISIT_BFS_FORWARD(discover);
			_CUBE_SCENE_VISIT_BFS_FORWARD(initialize);
			_CUBE_SCENE_VISIT_BFS_FORWARD(examine);
			_CUBE_SCENE_VISIT_BFS_FORWARD(finish);
# undef _CUBE_SCENE_VISIT_BFS_FORWARD
		};

	} // !detail

	struct DefaultBreadthFirstVisitor
	{
		void initialize_vertex(node::Node&) {}
		void discover_vertex(node::Node&) {}
		void examine_vertex(node::Node&) {}
		void finish_vertex(node::Node&) {}
	};

	template<typename Visitor>
	void breadth_first_search(Graph& graph, Visitor&& v)
	{
		BOOST_CONCEPT_ASSERT(( detail::BreadthFirstVisitorConcept<Visitor> ));
		detail::BreadthFirstVisitorWrapper<Visitor> wrapped(graph, v);

		boost::breadth_first_search(
			graph.impl().graph,
			graph.impl().nodes.at(&graph.root()),
			boost::visitor(wrapped)
				.vertex_index_map(graph.impl().vertex_index_map)
				.color_map(graph.impl().vertex_color_map)
		);
	}

}}}

#endif
