#include "tree.hpp"

#include <cube/debug.hpp>
#include <cube/gl/frustum.hpp>
#include <cube/gl/sphere.hpp>

namespace cubeapp { namespace core { namespace world { namespace tree {

	template<typename size_type>
	std::vector<Node<size_type>>
	find_nodes(Tree<size_type> const& tree,
	           cube::gl::vector::Vector3d const& pos,
	           cube::gl::frustum::Frustumd const& frustum)
	{
		CUBE_DEBUG_PERFORMANCE_SECTION("app.WorldTree");
		std::vector<Node<size_type>> res;
		typedef typename Tree<size_type>::vector_type vector_type;
		tree.visit(
			[&] (unsigned int level,
			     vector_type const& origin,
			     size_type const size)  {
				cube::gl::vector::Vector3d center{
					origin.x + size / 2 ,
					origin.y + size / 2 ,
					origin.z + size / 2 ,
				};

			cube::gl::sphere::Sphered s{
				center - pos,
				static_cast<double>(size) * 0.8660254037844386
			};
			if (not frustum.intersects(s))
				return VisitorAction::stop;
			if (level == 0)
                res.emplace_back(Node<size_type>{level, origin, size});
			return VisitorAction::continue_;
		});
		return res;
	}

	template
	std::vector<Node<int64_t>>
	find_nodes(Tree<int64_t> const& tree,
	           cube::gl::vector::Vector3d const& pos,
	           cube::gl::frustum::Frustumd const& frustum);

}}}}
