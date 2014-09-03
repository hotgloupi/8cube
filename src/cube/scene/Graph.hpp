#ifndef  CUBE_SCENE_GRAPH_HPP
# define CUBE_SCENE_GRAPH_HPP

# include "fwd.hpp"

# include <cube/api.hpp>

# include <etc/cast.hpp>
# include <etc/memory.hpp>
# include <etc/printable.hpp>

# include <iosfwd>
# include <functional>

namespace cube { namespace scene {

	/**
	 * Generic graph.
	 *
	 * Stores any kind of object in nodes
	 */
	class CUBE_API Graph
	{
	public:
		typedef node::Node Node;
		typedef std::function<void(Node*)> node_deleter_type;
		typedef node::Visitor<Node> visitor_type;
		typedef std::unique_ptr<visitor_type> visitor_ptr_type;
		enum class Event { insert, update, remove, _max_value };

	private:
		// Pimpl.
		struct Impl;
		std::unique_ptr<Impl> _this;
	public:
		Impl& impl() const ETC_NOEXCEPT { return *_this; }

	public:
		Graph();
		Graph(Graph&& other);
		Graph(std::unique_ptr<Node> node);
		Graph(Node& node, node_deleter_type deleter);
		~Graph();

	public:
		/// The graph root node
		Node& root() ETC_NOEXCEPT;

		/// Number of nodes (>= 1).
		size_t size() const ETC_NOEXCEPT;

		/// Insert a node into the graph.
		template<typename T>
		T& insert(std::unique_ptr<T> node)
		{ return etc::cast<T&>(_insert(etc::cast<Node>(std::move(node)))); }

		/// Insert a node owned outside of the graph.
		Node& insert(Node& node, std::function<void(Node*)> deleter);

		/// Insert a node as a child of `parent`.
		template<typename T>
		T& insert_child(Node& parent, std::unique_ptr<T> node)
		{
			T& n = this->insert(std::move(node));
			this->connect(parent, n);
			return n;
		}

		/// Create and insert a node into the graph.
		template<typename T, typename... Args>
		T& emplace(Args&&... args)
		{
			return this->insert(
				etc::make_unique<T>(std::forward<Args>(args)...)
			);
		}

		/// Create and insert a node as a child of `parent`.
		template<typename T, typename... Args>
		T& emplace_child(Node& parent, Args&&... args)
		{
			return this->insert_child(
				parent,
				etc::make_unique<T>(std::forward<Args>(args)...)
			);
		}

		/// Add a link between nodes `from` and `to`.
		void connect(Node& from, Node& to);

		/// Remove a node and returns it.
		std::shared_ptr<Node> remove(Node& node);

		/// Return child nodes.
		std::vector<Node*> children(Node& node);

	public:
		void traverse(visitor_type& visitor);

	public:
		void breadth_first_search(visitor_type& visitor);
		void depth_first_search(visitor_type& visitor);

		struct hook_guard
		{
		private:
			Graph& _graph;
			visitor_type* _visitor;
			Event const _event;

		public:
			hook_guard(Graph& graph, visitor_type& visitor, Event const event)
				: _graph(graph)
				, _visitor{&visitor}
				, _event{event}
			{}

			hook_guard(hook_guard&& other)
				: _graph(other._graph)
				, _visitor{std::move(other._visitor)}
				, _event{other._event}
			{ other._visitor = nullptr; }

			~hook_guard()
			{ if (_visitor != nullptr) _graph._remove_hook(_event, *_visitor); }
		};
		template<typename T>
		hook_guard add_hook(Event const ev, std::unique_ptr<T> visitor)
		{
			return _add_hook(
				ev,
				etc::cast<visitor_type>(std::move(visitor))
			);
		}
		hook_guard _add_hook(Event const ev, visitor_ptr_type visitor);
		void _remove_hook(Event const ev, visitor_type& visitor);


	private:
		Node& _insert(std::unique_ptr<Node> node);
	};

	CUBE_API
	std::ostream& operator <<(std::ostream& out, Graph::Event const e);

}}

#endif
