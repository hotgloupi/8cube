#ifndef  CUBEAPP_CORE_WORLD_TREE_HPP
# define CUBEAPP_CORE_WORLD_TREE_HPP

# include <cubeapp/api.hpp>

# include <cube/gl/vector.hpp>
# include <cube/gl/fwd.hpp>

# include <etc/meta/math/power.hpp>
# include <etc/types.hpp>

# include <cstdlib>
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

#define LEVEL_TO_SIZE(size_type, level) (size_type{1} << (level))
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

		//template<level_type level>
		//using level_to_size = etc::meta::math::power<size_type, 2, level>;

	public:
		static level_type const max_level = sizeof(size_type) * 8 - 1;
	private:
		level_type const _root_level;
		vector_type const _root_origin;

	public:
		Tree(level_type const root_level) ETC_NOEXCEPT
			: _root_level{root_level}
			, _root_origin(
				(std::numeric_limits<size_type>::max()
				- std::numeric_limits<size_type>::min()) / 2
				- (size_type{1} << (root_level - 1)) // 2^(root_level - 1)
			)
		{}

	private:
		template<typename Visitor>
		using visit_method_type = void(*)(vector_type const&, Visitor&&);

	public:
		template<typename Visitor>
		inline
		void visit(Visitor&& visitor) const ETC_NOEXCEPT
		{
			static auto visit_method = _find_visit_method<Tree::max_level - 1, Visitor>();
			visit_method(_root_origin, std::forward<Visitor>(visitor));
		}

	private:
		template<level_type level, typename Visitor>
		typename std::enable_if<level != 0, void(*)(vector_type const&, Visitor&&)>::type
		_find_visit_method() const ETC_NOEXCEPT
		{
			if (_root_level == level)
				return &Tree::_visit_node<level, Visitor>;
			else
				return _find_visit_method<level - 1, Visitor>();
		}

		template<level_type level, typename Visitor>
		typename std::enable_if<level == 0, void(*)(vector_type const&, Visitor&&)>::type
		_find_visit_method() const ETC_NOEXCEPT { std::abort(); return nullptr; }

		template<level_type level, typename Visitor>
		static inline
		void _visit_node(vector_type const& origin,
		                 Visitor&& visitor) ETC_NOEXCEPT
		{
			if (visitor(level, origin, LEVEL_TO_SIZE(size_type, level)) == VisitorAction::continue_)
				_visit_children<level>(origin, std::forward<Visitor>(visitor));
		}

		template<level_type level, typename Visitor>
		static inline
		typename std::enable_if<level != 0>::type
		_visit_children(vector_type const& origin,
		                Visitor&& visitor) ETC_NOEXCEPT
		{
			static size_type const child_size = LEVEL_TO_SIZE(size_type, level - 1);
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
		_visit_children(vector_type const&, Visitor&&) ETC_NOEXCEPT
		{ /* level 0 has no children */ }
	};

	template<
		  typename size_type
		, typename vector_type = typename Tree<size_type>::vector_type
	>
	struct CUBEAPP_API Node
	{
		unsigned int const level;
		vector_type const origin;
		size_type const size;
	};

	template<typename size_type>
	CUBEAPP_API
	std::vector<Node<size_type>>
	find_nodes(Tree<size_type> const& tree,
	           cube::gl::vector::Vector3d const& pos,
	           cube::gl::frustum::Frustumd const& frustum) ETC_NOEXCEPT;

	template<typename size_type>
	CUBEAPP_API
	std::ostream& operator <<(std::ostream& out, Node<size_type> const& node);

}}}}

#endif
