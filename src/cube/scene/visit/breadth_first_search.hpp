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
				Node* node;
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
			Visitor& _visitor;
			BreadthFirstVisitorWrapper(Visitor& visitor) : _visitor(visitor) {}
			BreadthFirstVisitorWrapper(BreadthFirstVisitorWrapper const& other)
				: _visitor(other._visitor)
			{}
# define _CUBE_SCENE_VISIT_BFS_FORWARD(__name)                                \
			template<typename V, typename G>                                  \
			void __name ## _vertex(V vertex, G const& g)                      \
			{ _visitor.__name ## _vertex(*g[vertex]); }                       \
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
		void initialize_vertex(Node&) {}
		void discover_vertex(Node&) {}
		void examine_vertex(Node&) {}
		void finish_vertex(Node&) {}
	};

	template<typename Visitor>
	void breadth_first_search(Graph& graph, Visitor& v)
	{
		BOOST_CONCEPT_ASSERT(( detail::BreadthFirstVisitorConcept<Visitor> ));
		auto& graph_impl = graph.impl().graph;
		detail::BreadthFirstVisitorWrapper<Visitor> wrapped(v);

		boost::breadth_first_search(
			graph_impl,
			graph.root().id(),
			boost::visitor(wrapped)
		);
	}

}}}

#endif
