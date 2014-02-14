#ifndef  CUBE_SCENE_NODE_HPP
# define CUBE_SCENE_NODE_HPP

# include "fwd.hpp"
# include "NodeVisitor.hpp"

# include <cube/api.hpp>

# include <etc/cast.hpp>
# include <etc/memory.hpp>
# include <etc/printable.hpp>
# include <etc/log/component.hpp>

# include <boost/noncopyable.hpp>

# include <memory>
# include <string>
# include <map>

namespace cube { namespace scene {

	class CUBE_API Node
		: public VisitableNode<Node>
		, public etc::Printable
		, private boost::noncopyable
	{
		ETC_LOG_COMPONENT("cube.scene.Node");
	public:
		typedef size_t id_type;

	protected:
		std::string _name;
		Graph*      _graph;
		id_type     _id;

	public:
		/// Construct a Node with a name.
		explicit Node(std::string name);

		/// Node class is meant to be subclassed.
		virtual ~Node();

	public:
		/// Get the name.
		std::string const& name() const ETC_NOEXCEPT { return _name; }

		/// Set the name.
		Node& name(std::string name) { _name = std::move(name); return *this; }

		/// Whether or not the node is attached to a graph.
		bool attached() const ETC_NOEXCEPT { return _graph != nullptr; }

		/// Get the node id. If the node has not been attached to any graph,
		/// throws an exception.
		id_type id() const;

		/// Get the graph containing this node or throw an error.
		Graph& graph() const;

		/// Attach a node into the graph, ensuring that the node is not already
		/// attached to another graph.
		void attach(Graph& g, id_type const id);

		/// Detach a node from its graph. id() and graph() methods will throw.
		void detach(Graph& g);

		template<typename NodeType, typename... Args>
		NodeType& emplace(Args&&... args)
		{
			return this->insert_child(
				etc::make_unique<NodeType>(std::forward<Args>(args)...)
			);
		}

		template<typename T>
		T& insert_child(std::unique_ptr<T> node)
		{ return etc::cast<T&>(_insert_child(etc::cast<Node>(std::move(node)))); }

	private:
		Node& _insert_child(std::unique_ptr<Node> node);

	public:
		using VisitableNode<Node>::visit;

	public:
		// Implement printable interface.
		void print(std::ostream& out) const ETC_NOEXCEPT override;
	};

}}

#endif
