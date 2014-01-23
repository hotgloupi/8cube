#ifndef  CUBE_SCENE_NODE_HPP
# define CUBE_SCENE_NODE_HPP

# include "fwd.hpp"
# include "NodeVisitor.hpp"

# include <cube/api.hpp>

# include <etc/Printable.hpp>

# include <boost/noncopyable.hpp>

# include <memory>
# include <string>
# include <map>

namespace cube { namespace scene {

	class CUBE_API Node
		: public std::enable_shared_from_this<Node>
		, public VisitableNode<Node>
		, public etc::Printable
		, private boost::noncopyable
	{
	protected:
		typedef std::weak_ptr<GroupNode> parent_node_type;

	protected:
		std::string                       _name;
		Graph&                            _graph;
		std::map<Node*, parent_node_type> _parents;

	public:
		explicit Node(Graph& graph, std::string name);
		virtual ~Node();

	public:
		/// Get the name.
		std::string const& name() const ETC_NOEXCEPT { return _name; }

		/// Set the name.
		Node& name(std::string name) { _name = std::move(name); return *this; }

	public:
		/// Graph owning this node.
		Graph& graph() const ETC_NOEXCEPT { return _graph; }

	public:
		// Implement printable interface.
		void print(std::ostream& out) const ETC_NOEXCEPT override;

	protected:
		friend class GroupNode;
		void _add_parent(GroupNode& parent);
		void _remove_parent(GroupNode& parent);
	};

}}

#endif
