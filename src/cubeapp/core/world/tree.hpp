#ifndef  CUBEAPP_CORE_WORLD_TREE_HPP
# define CUBEAPP_CORE_WORLD_TREE_HPP

# include <cube/gl/vector.hpp>

# include <etc/meta/math/power.hpp>
# include <etc/types.hpp>

# include <cstring>
# include <limits>
# include <tuple>
# include <vector>
# include <iosfwd>

namespace cubeapp { namespace core { namespace world { namespace tree {

	/// Return type for the visitor.
	enum class VisitorAction
	{
		stop,
		continue_,
	};

	template<
		  typename size_type_
		, typename vector_type_ = cube::gl::vector::Vector3<size_type_>
	>
	struct Tree
	{
	public:
		typedef size_type_                                      size_type;
		typedef typename std::make_unsigned<size_type>::type    usize_type;
		typedef vector_type_                                    vector_type;
		typedef unsigned int                                    level_type;

		template<level_type level>
		using level_to_size = etc::meta::math::power<size_type, 2, level>;

	private:

	public:
		static level_type const max_level = sizeof(size_type) * 8 - 1;
	private:
		level_type const _root_level;
		vector_type const _root_origin;

	public:
		Tree(level_type const root_level)
			: _root_level{root_level}
			, _root_origin{
				(std::numeric_limits<size_type>::max()
				 - std::numeric_limits<size_type>::min()) / 2
				- (size_type{1} << (root_level - 1)) // 2^(root_level - 1)
			}
		{

		}

	private:
		template<typename Visitor>
		using visit_method_type = void(*)(vector_type const&, Visitor&&);

	public:
		template<typename Visitor>
		inline
		void visit(Visitor&& visitor) const
		{
			static visit_method_type<Visitor> visit_method =
				_find_visit_method<Tree::max_level, Visitor>();
			vector_type const& o = _root_origin;
			visit_method(o, std::forward<Visitor>(visitor));
		}

	private:
		template<level_type level, typename Visitor>
		typename std::enable_if<level != 0, visit_method_type<Visitor>>::type
		_find_visit_method() const
		{
			if (_root_level == level)
				return &Tree::_visit_node<level, Visitor>;
			else
				return _find_visit_method<level - 1, Visitor>();
		}

		template<level_type level, typename Visitor>
		typename std::enable_if<level == 0, visit_method_type<Visitor>>::type
		_find_visit_method() const { std::abort(); return nullptr; }

		template<level_type level, typename Visitor>
		static inline
		void _visit_node(vector_type const& origin,
		                 Visitor&& visitor)
		{
			if (visitor(origin, level_to_size<level>::value) == VisitorAction::continue_)
				_visit_children(origin, std::forward<Visitor>(visitor));
		}

		template<level_type level, typename Visitor>
		static inline
		typename std::enable_if<level != 0>::type
		_visit_children(vector_type const& origin,
		                Visitor&& visitor)
		{
			static size_type const child_size = level_to_size<level - 1>::value;
			static vector_type const children_offsets[] = {
				vector_type{0,          0,          0},
				vector_type{0,          0,          child_size},
				vector_type{0,          child_size, 0},
				vector_type{0,          child_size, child_size},
				vector_type{child_size, 0,          0},
				vector_type{child_size, 0,          child_size},
				vector_type{child_size, child_size, 0},
				vector_type{child_size, child_size, child_size},
			};
			for (auto const& offset: children_offsets)
			{
				_visit_node<level - 1>(
					origin + offset,
					std::forward<Visitor>(visitor)
				);
			}
		}

		template<level_type level, typename Visitor>
		static inline
		typename std::enable_if<level == 0>::type
		_visit_children(vector_type const&, Visitor&&)
		{ /* level 0 has no children */ }
	};


//	/// world coordinate type
//	typedef int64_t                                 size_type;
//	typedef std::make_unsigned<size_type>::type     usize_type;
//
//	/// Point in the world
//	typedef cube::gl::vector::Vector3<size_type>    vector_type;
//
//	/// The scale factor type
//	typedef unsigned int                            level_type;
//
//	/// Return type for the visitor.
//	enum class VisitAction
//	{
//		stop,
//		continue_,
//		stop_and_clean,
//		continue_and_clean,
//	};
//
//	/// Visit mode determines call arguments for the visitor.
//	enum class VisitMode
//	{
//		node,
//		node_content,
//	};
//
//	template<level_type level_>
//	struct Node;
//
//	// Common values for node type
//	template<level_type level_>
//	struct NodeCommon
//	{
//	public:
//		static level_type const level = level_;
//		static usize_type const size =
//			etc::meta::math::power<usize_type, 2, level_>::value;
//
//	public:
//		vector_type const   origin;
//
//	public:
//		explicit
//		NodeCommon(vector_type const& origin)
//			: origin{origin}
//		{}
//
//		inline
//		bool contains(vector_type const& point) const
//		{
//			return (
//				   point.x >= this->origin.x && point.x < this->origin.x + static_cast<size_type>(this->size)
//				&& point.y >= this->origin.y && point.y < this->origin.y + static_cast<size_type>(this->size)
//				&& point.z >= this->origin.z && point.z < this->origin.z + static_cast<size_type>(this->size)
//			);
//
//			/*return (
//				   static_cast<usize_type>(glm::abs(point.x - this->origin.x)) < this->size
//				&& static_cast<usize_type>(glm::abs(point.y - this->origin.y)) < this->size
//				&& static_cast<usize_type>(glm::abs(point.z - this->origin.z)) < this->size
//			);*/
//		}
//
//	protected:
//		template<VisitMode mode, typename Visitor>
//		inline
//		typename std::enable_if<mode == VisitMode::node, VisitAction>::type
//		_call_visitor(Visitor&& visitor)
//		{
//			return visitor.template visit<level_>(
//					static_cast<Node<level_>&>(*this)
//			);
//		}
//
//		template<VisitMode mode, typename Visitor>
//		inline
//		typename std::enable_if<mode == VisitMode::node_content, VisitAction>::type
//		_call_visitor(Visitor&& visitor)
//		{
//			return visitor.template visit<level_>(this->origin, this->size);
//		}
//	};
//
//	/// The node type
//	template<level_type level>
//	struct Node : NodeCommon<level>
//	{
//	public:
//		typedef Node<level - 1> ChildType;
//
//	private:
//		ChildType*          _children[8];
//
//	public:
//		explicit
//		Node(vector_type const& origin)
//			: NodeCommon<level>{origin}
//			, _children{nullptr}
//		{}
//
//		template<VisitMode mode, typename Visitor>
//		inline
//		void visit(Visitor&& visitor)
//		{
//			auto action = this->template _call_visitor<mode>(std::forward<Visitor>(visitor));
//			if (action == VisitAction::stop)
//				return;
//			else if (action == VisitAction::stop_and_clean)
//				return _clean();
//
//			_ensure_children();
//
//			for (ChildType* child: _children)
//				child->template visit<mode>(std::forward<Visitor>(visitor));
//			if (action == VisitAction::continue_and_clean)
//				_clean();
//			else
//				assert(action == VisitAction::continue_);
//		}
//
//		~Node()
//		{ _clean(); }
//
//	private:
//		inline
//		void _clean()
//		{
//			if (_children[0] == nullptr)
//				return;
//			for (ChildType* child: _children)
//				delete child;
//			std::memset(_children, 0, sizeof(_children));
//		}
//
//		inline
//		void _ensure_children()
//		{
//			if (_children[0] != nullptr)
//				return;
//			auto s = this->size / 2;
//			_children[7] = new ChildType{this->origin + vector_type{0, 0, 0}};
//			_children[6] = new ChildType{this->origin + vector_type{0, 0, s}};
//			_children[5] = new ChildType{this->origin + vector_type{0, s, 0}};
//			_children[4] = new ChildType{this->origin + vector_type{0, s, s}};
//			_children[3] = new ChildType{this->origin + vector_type{s, 0, 0}};
//			_children[2] = new ChildType{this->origin + vector_type{s, 0, s}};
//			_children[1] = new ChildType{this->origin + vector_type{s, s, 0}};
//			_children[0] = new ChildType{this->origin + vector_type{s, s, s}};
//		}
//	};
//
//	/// Specialiatation for leaf node
//	template<>
//	struct Node<0> : NodeCommon<0>
//	{
//	public:
//		explicit
//		Node(vector_type const& origin)
//			: NodeCommon<0>{origin}
//		{}
//
//		template<VisitMode mode, typename Visitor>
//		inline
//		void visit(Visitor&& visitor)
//		{
//			(void) _call_visitor<mode>(std::forward<Visitor>(visitor));
//		}
//	};
//
//
//	/// Tree node
//	struct Tree
//	{
//		static level_type const max_exponent = 7;//sizeof(size_type) * 8 - 1;
//	private:
//		Node<max_exponent>* _root;
//
//	public:
//		Tree();
//		~Tree();
//
//		template<typename Visitor>
//		void visit(Visitor&& visitor)
//		{
//			_root->visit<VisitMode::node_content>(visitor);
//		}
//
//		template<typename Visitor>
//		void visit_nodes(Visitor&& visitor)
//		{
//			_root->visit<VisitMode::node>(visitor);
//		}
//	};
//
//	namespace detail {
//
//		// Helper to generate a tuple type
//		template<level_type level, typename... Args>
//		struct NodeListArray
//		{
//			typedef
//				typename NodeListArray<
//					  level - 1
//					, std::vector<Node<level>*>
//					, Args...
//				>::type
//				type;
//		};
//
//		template<typename... Args>
//		struct NodeListArray<0, Args...>
//		{
//			typedef std::tuple<std::vector<Node<0>*>, Args...> type;
//		};
//
//	} // !detail
//
//	/// A simple structure to aggregate nodes
//	template<level_type max_level>
//	struct NodeList
//	{
//		typename detail::NodeListArray<max_level>::type array;
//		template<level_type level>
//		void append(Node<level>& node)
//		{
//			static_assert(level <= max_level, "??");
//			std::get<level>(array).push_back(&node);
//		}
//	};

}}}}

#endif
