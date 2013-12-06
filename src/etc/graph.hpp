#ifndef ETC_GRAPH_HPP
# define ETC_GRAPH_HPP

# include <cassert>
# include <memory>
# include <set>

namespace etc {

	template<typename Value>
	struct Node
		: public std::enable_shared_from_this<Node<Value>>
	{
		typedef Value value_type;
		typedef std::shared_ptr<value_type> shared_node_type;
		typedef std::weak_ptr<value_type> weak_node_type;

		std::set<weak_node_type> parents;
		std::set<shared_node_type> children;

		inline
		void add_child(shared_node_type& node) ETC_NOEXCEPT
		{
			assert(node != nullptr);
			auto res = this->children.insert(node);
			if (res.second == false)
				return;
			node->add_parent(this->shared_from_this());
		}

		inline
		void add_parent(shared_node_type& node) ETC_NOEXCEPT
		{
			assert(node != nullptr);
			auto res = this->parents.insert(node);
			if (res.second == false)
				return;
			node->add_child(this->shared_from_this());
		}

		inline
		void remove_child(shared_node_type& node) ETC_NOEXCEPT
		{
			assert(node != nullptr);
			if (this->children.erase(node) == 0)
				return;
			node->remove_parent(this->shared_from_this());
		}

		inline
		void remove_parent(shared_node_type& node) ETC_NOEXCEPT
		{
			assert(node != nullptr);
			if (this->parents.erase(node) == 0)
				return;
			node->remove_child(this->shared_from_this());
		}
	};

	template<typename Node>
	struct Graph
	{
		typedef Node node_type;
		typedef typename Node::value_type value_type;
	};

}

#endif
