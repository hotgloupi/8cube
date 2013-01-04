#ifndef  CUBEAPP_CORE_WORLD_TREE_HPP
# define CUBEAPP_CORE_WORLD_TREE_HPP

# include <cube/gl/vector.hpp>

# include <etc/meta/math/power.hpp>
# include <etc/types.hpp>

# include <cstring>
# include <limits>
# include <tuple>
# include <vector>

# ifdef CUBEAPP_DEBUG
#  include <iostream>
# endif

namespace cubeapp { namespace core { namespace world { namespace tree {

	/// world coordinate type
	typedef int64_t                                 size_type;
	typedef std::make_unsigned<size_type>::type     usize_type;

	/// Point in the world
	typedef cube::gl::vector::Vector3<size_type>    vector_type;

	/// The scale factor type
	typedef unsigned int                            level_type;

	/// Return type for the visitor.
	enum class VisitAction
	{
		stop,
		continue_,
		stop_and_clean,
		continue_and_clean,
	};

	/// Visit mode determines call arguments for the visitor.
	enum class VisitMode
	{
		node,
		node_content,
	};

	template<level_type level_>
	struct Node;

	// Common values for node type
	template<level_type level_>
	struct NodeCommon
	{
	public:
		static level_type const level = level_;
		static usize_type const size =
			etc::meta::math::power<usize_type, 2, level_>::value;

	public:
		vector_type const   origin;

	public:
		explicit
		NodeCommon(vector_type const& origin)
			: origin{origin}
		{}

		inline
		bool contains(vector_type const& point) const
		{
			/*
			return (
				   point.x >= this->origin.x && point.x < this->origin.x + this->size
				&& point.y >= this->origin.y && point.y < this->origin.y + this->size
				&& point.z >= this->origin.z && point.z < this->origin.z + this->size
			);
			*/
			return (
				   static_cast<usize_type>(glm::abs(point.x - this->origin.x)) < this->size
				&& static_cast<usize_type>(glm::abs(point.y - this->origin.y)) < this->size
				&& static_cast<usize_type>(glm::abs(point.z - this->origin.z)) < this->size
			);
		}

	protected:
		template<VisitMode mode, typename Visitor>
		inline
		typename std::enable_if<mode == VisitMode::node, VisitAction>::type
		_call_visitor(Visitor&& visitor)
		{
			return visitor.template visit<level_>(
					static_cast<Node<level_>&>(*this)
			);
		}

		template<VisitMode mode, typename Visitor>
		inline
		typename std::enable_if<mode == VisitMode::node_content, VisitAction>::type
		_call_visitor(Visitor&& visitor)
		{
			return visitor.template visit<level_>(this->origin, this->size);
		}
	};

	/// The node type
	template<level_type level>
	struct Node : NodeCommon<level>
	{
	public:
		typedef Node<level - 1> ChildType;

	private:
		ChildType*          _children[8];

	public:
		explicit
		Node(vector_type const& origin)
			: NodeCommon<level>{origin}
			, _children{nullptr}
		{}

		template<VisitMode mode, typename Visitor>
		inline
		void visit(Visitor&& visitor)
		{
			auto action = this->template _call_visitor<mode>(std::forward<Visitor>(visitor));
			if (action == VisitAction::stop)
				return;
			else if (action == VisitAction::stop_and_clean)
				return _clean();

			_ensure_children();

			for (ChildType* child: _children)
				child->template visit<mode>(std::forward<Visitor>(visitor));
			if (action == VisitAction::continue_and_clean)
				_clean();
			else
				assert(action == VisitAction::continue_);
		}

		~Node()
		{ _clean(); }

	private:
		inline
		void _clean()
		{
			if (_children[0] == nullptr)
				return;
			for (ChildType* child: _children)
				delete child;
			std::memset(_children, 0, sizeof(_children));
		}

		inline
		void _ensure_children()
		{
			if (_children[0] != nullptr)
				return;
			auto s = this->size / 2;
			_children[0] = new ChildType{this->origin + vector_type{0, 0, 0}};
			_children[1] = new ChildType{this->origin + vector_type{0, 0, s}};
			_children[2] = new ChildType{this->origin + vector_type{0, s, 0}};
			_children[3] = new ChildType{this->origin + vector_type{0, s, s}};
			_children[4] = new ChildType{this->origin + vector_type{s, 0, 0}};
			_children[5] = new ChildType{this->origin + vector_type{s, 0, s}};
			_children[6] = new ChildType{this->origin + vector_type{s, s, 0}};
			_children[7] = new ChildType{this->origin + vector_type{s, s, s}};
		}
	};

	/// Specialiatation for leaf node
	template<>
	struct Node<0> : NodeCommon<0>
	{
	public:
		explicit
		Node(vector_type const& origin)
			: NodeCommon<0>{origin}
		{}

		template<VisitMode mode, typename Visitor>
		inline
		void visit(Visitor&& visitor)
		{
			(void) _call_visitor<mode>(std::forward<Visitor>(visitor));
		}
	};


	/// Tree node
	struct Tree
	{
		static level_type const max_exponent = sizeof(size_type) * 8 - 1;
	private:
		Node<max_exponent>* _root;

	public:
		Tree();
		~Tree();

		template<typename Visitor>
		void visit(Visitor&& visitor)
		{
			_root->visit<VisitMode::node_content>(visitor);
		}

		template<typename Visitor>
		void visit_nodes(Visitor&& visitor)
		{
			_root->visit<VisitMode::node>(visitor);
		}
	};

	namespace detail {

		// Helper to generate a tuple type
		template<level_type level, typename... Args>
		struct NodeListArray
		{
			typedef
				typename NodeListArray<
					  level - 1
					, std::vector<Node<level>*>
					, Args...
				>::type
				type;
		};

		template<typename... Args>
		struct NodeListArray<0, Args...>
		{
			typedef std::tuple<std::vector<Node<0>*>, Args...> type;
		};

	} // !detail

	/// A simple structure to aggregate nodes
	template<level_type max_level>
	struct NodeList
	{
		typename detail::NodeListArray<max_level>::type array;
		template<level_type level>
		void append(Node<level>& node)
		{
			static_assert(level <= max_level, "??");
			std::get<level>(array).push_back(&node);
		}
	};

}}}}

#endif
