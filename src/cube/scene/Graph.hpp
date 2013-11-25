#ifndef  CUBE_SCENE_GRAPH_HPP
# define CUBE_SCENE_GRAPH_HPP

# include <cube/api.hpp>

# include <etc/memory.hpp>
# include <etc/printable.hpp>

# include <iosfwd>

namespace cube { namespace scene {

	class CUBE_API Graph
	{
	public:
		class Node;
		typedef unsigned long int id_type;
	private:
		struct Impl;
		std::unique_ptr<Impl> _this;
		Node& _root;

	public:
		Graph();
		~Graph();

	public:
		/// The graph root node its id equals 0, and it's data is nullptr.
		inline
		Node& root() noexcept
		{ return _root; }

	public:
		/// Number of nodes (>= 1).
		size_t size() const noexcept;

	public:
		template<typename T>
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
		typename std::enable_if<not std::is_integral<T>::value, Node&>::type
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
		void _remove(Node const& node) noexcept;
	};

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
		Node(Graph& graph, void* data) noexcept
			: _graph(graph)
			, _data{data}
			, _id{0}
		{}
		virtual ~Node();

	public:
		inline Node& id(id_type id) noexcept { _id = id; return *this; }
		inline id_type id() const noexcept { return _id; }

	public:
		inline
		void const* data() const noexcept { return _data; }

	public:
		template<typename T>
		Node& add_child(T& object)
		{ return _graph.add(object, this); }

	public:
		void print(std::ostream& out) const noexcept override;
	protected:
		void _begin_print(std::ostream& out) const noexcept;
		void _end_print(std::ostream& out) const noexcept;
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
		void print(std::ostream& out) const noexcept override
		{
			Node::_begin_print(out);
			static_cast<T const*>(this->data())->print(out);
			Node::_end_print(out);
		}
	};

}}

#endif
