#ifndef CUBE_SCENE_GROUPNODE_HPP
# define CUBE_SCENE_GROUPNODE_HPP

# include "Node.hpp"

# include <etc/memory.hpp>

# include <map>

namespace cube { namespace scene {

	class GroupNode
		: public Node
		, public VisitableNode<GroupNode>
	{
	protected:
		typedef std::shared_ptr<Node> child_node_type;

	private:
		std::map<Node*, child_node_type> _children;

	public:
		explicit GroupNode(Graph& graph, std::string name);

	public:
		GroupNode& name(std::string name)
		{ Node::name(std::move(name)); return *this; }

	public:
		using VisitableNode<GroupNode>::visit;

		void visit_children(NodeVisitor<Node>& visitor)
		{
			for (auto& pair: _children)
				visitor.visit(*pair.first);
		}

	public:
		/// Add a child and returns it.
		template<typename T>
		T& add(std::unique_ptr<T> child)
		{
			_add(NodePtr{child.get()});
			return *child.release();
		}

	private:
		void _add(NodePtr child);

	public:
		/// Construct and add a new child and returns it.
		/// The graph is automatically prepended to the argument list.
		template<typename T, typename... Args>
		T& emplace(Args&&... args)
		{
			return this->add(
				etc::make_unique<T>(
					_graph,
					std::forward<Args>(args)...
				)
			);
		}

		GroupNode& remove_child(Node& child);

		bool has_child(Node& child) const ETC_NOEXCEPT;

	public:
		void print(std::ostream& out) const ETC_NOEXCEPT override;
	};

}}

#endif
