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
	private:
		// Pimpl.
		struct Impl;
		std::unique_ptr<Impl> _this;

	public:
		Graph();
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
		std::unique_ptr<Node> remove(Node& node);

	public:
		void traverse(NodeVisitor<Node>& visitor);

	private:
		Node& _insert(std::unique_ptr<Node> node);
	};

}}

#endif
