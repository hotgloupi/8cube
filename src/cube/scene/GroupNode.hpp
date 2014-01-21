#ifndef CUBE_SCENE_GROUPNODE_HPP
# define CUBE_SCENE_GROUPNODE_HPP

# include "Node.hpp"

# include <vector>

namespace cube { namespace scene {

	class GroupNode
		: public Node
	{
	private:
		std::vector<NodePtr> _children;

	public:
		explicit GroupNode(Graph& graph, std::string name);

	public:
		GroupNode& name(std::string name)
		{ Node::name(std::move(name)); return *this; }

	public:
		GroupNode& add_child(NodePtr child);

		GroupNode& remove_child(Node& child);

		bool has_child(Node& child) const ETC_NOEXCEPT;

	public:
		void print(std::ostream& out) const ETC_NOEXCEPT override;
	};

}}

#endif
