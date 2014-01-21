#ifndef  CUBE_SCENE_GRAPH_HPP
# define CUBE_SCENE_GRAPH_HPP

# include "fwd.hpp"

# include <cube/api.hpp>

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
		GroupNode& root() ETC_NOEXCEPT;

		/// Number of nodes (>= 1).
		size_t size() const ETC_NOEXCEPT;

	public:
		void dump();

	private:
		// Nodes registers to the graph on creation, and unregister on dtor.
		friend class Node;
		void _node_register(Node& node);
		void _node_unregister(Node& node);
	};

}}

#endif
