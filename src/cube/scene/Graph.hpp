#ifndef  CUBE_SCENE_GRAPH_HPP
# define CUBE_SCENE_GRAPH_HPP

# include <cube/api.hpp>

# include <etc/memory.hpp>
# include <etc/printable.hpp>

# include <iosfwd>

namespace cube { namespace scene {

	/**
	 * Generic graph.
	 *
	 * Stores any kind of object in nodes
	 */
	class CUBE_API Graph
	{
	public:
		/// Node interface.
		class Node;

		/// Node id type.
		typedef size_t id_type;

	private:
		// Pimpl.
		struct Impl;
		std::unique_ptr<Impl> _this;

	private:
		// Root node.
		Node& _root;

	public:
		Graph();
		~Graph();

	public:
		/// The graph root node its id equals 0, and its data is nullptr.
		inline
		Node& root() ETC_NOEXCEPT
		{ return _root; }

	public:
		/// Number of nodes (>= 1).
		size_t size() const ETC_NOEXCEPT;

	public:
		/// Add an object to the graph and returns the created node.
		/// @param object The value associated to the node. Its lifetime should
		/// be longer than the node's one.
		template<typename T>
		inline
		Node& add(T& object, Node* parent = nullptr)
		{
			return this->_add(
				std::unique_ptr<Node>{new NodeAdaptor<T>{*this, object}},
				parent
			);
		}

	public:
		Node& by_data(void const* data);

		template<typename T>
		inline
		typename std::enable_if<!std::is_integral<T>::value, Node&>::type
		operator [](T const& object)
		{ return this->by_data(&object); }

	public:
		Node& by_id(id_type const id);

		template<typename T>
		inline
		typename std::enable_if<std::is_integral<T>::value, Node&>::type
		operator [](T const id)
		{ return this->by_id(id); }


		void dump();
	private:
		template<typename T>
		struct NodeAdaptor;
		Node& _add(std::unique_ptr<Node> node, Node* parent);
		void _remove(Node const& node) ETC_NOEXCEPT;
	};

	/**
	 * Hold some data in the graph.
	 */
	class Graph::Node
		: public etc::Printable
	{
	public:
		typedef Graph::id_type id_type;
	private:
		Graph&      _graph;
		void*       _data;
		id_type     _id;

	public:
		inline explicit
		Node(Graph& graph, void* data) ETC_NOEXCEPT
			: _graph(graph)
			, _data{data}
			, _id{0}
		{}
		virtual ~Node();

	public:
		inline Node& id(id_type id) ETC_NOEXCEPT { _id = id; return *this; }
		inline id_type id() const ETC_NOEXCEPT { return _id; }

	public:
		inline
		void const* data() const ETC_NOEXCEPT { return _data; }

	public:
		template<typename T>
		Node& add_child(T& object)
		{ return _graph.add(object, this); }

	public:
		void print(std::ostream& out) const ETC_NOEXCEPT override;
	protected:
		void _begin_print(std::ostream& out) const ETC_NOEXCEPT;
		void _end_print(std::ostream& out) const ETC_NOEXCEPT;
	};

	template<typename T>
	struct Graph::NodeAdaptor
		: public Graph::Node
	{
		inline
		NodeAdaptor(Graph& graph, T& object)
			: Node{graph, &object}
		{}

		inline
		void print(std::ostream& out) const ETC_NOEXCEPT override
		{
			Node::_begin_print(out);
			static_cast<T const*>(this->data())->print(out);
			Node::_end_print(out);
		}
	};

}}

#endif
