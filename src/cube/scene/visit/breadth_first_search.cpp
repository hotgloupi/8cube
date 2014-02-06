#include "breadth_first_search.hpp"

#include <etc/test.hpp>

namespace {

	using namespace cube::scene;

	struct BFSVisitor
		: public visit::DefaultBreadthFirstVisitor
	{
		std::set<Node*> visited;
		void discover_vertex(Node& n) { visited.insert(&n); }
	};

	ETC_TEST_CASE(simple)
	{
		Graph g;
		auto& lol = g.emplace_child<Node>(g.root(), "LOL!");
		g.emplace_child<Node>(lol, "last one");
		g.root().emplace<Node>("right").emplace<Node>("last child");
		BFSVisitor visitor;
		visit::breadth_first_search(g, visitor);
		ETC_ENFORCE_EQ(visitor.visited.size(), 5u);
	}

}
