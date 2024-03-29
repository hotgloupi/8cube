#include "tree.hpp"

#include <cube/debug.hpp>
#include <cube/gl/frustum.hpp>
#include <cube/gl/sphere.hpp>

#include <etc/log.hpp>

#include <boost/thread.hpp>

#include <iostream>

#define MAX_ITER 10000
#define MAX_NODES 1000

namespace cubeapp { namespace world { namespace tree {

	ETC_LOG_COMPONENT("cubeapp.core.world.Tree");

	template<typename size_type>
	std::vector<Node<size_type>>
	find_nodes(Tree<size_type> const& tree,
	           cube::gl::vector::Vector3d const& pos,
	           cube::gl::frustum::Frustumd const& frustum) ETC_NOEXCEPT
	{
		CUBE_DEBUG_PERFORMANCE_SECTION("app.WorldTree");
		std::vector<Node<size_type>> res;
		typedef typename Tree<size_type>::vector_type vector_type;
		etc::size_type i = 0;
		tree.visit(
			[&] (unsigned int level,
			     vector_type const& origin,
			     size_type const size)  {

				if (i >= MAX_ITER || res.size() >= MAX_NODES)
					return VisitorAction::stop;
				i += 1;

				cube::gl::vector::Vector3d center{
					origin.x + size / 2 ,
					origin.y + size / 2 ,
					origin.z + size / 2 ,
				};

				cube::gl::sphere::Sphered s{
					center - pos,
					static_cast<double>(size) * 0.8660254037844386
				};
				if (!frustum.intersects(s))
					return VisitorAction::stop;
				if (s.radius * 2 < glm::length(s.center))
				{
					if (level < 9)
						res.emplace_back(Node<size_type>{origin, size});
					return VisitorAction::stop;
				}
				//boost::this_thread::sleep_for(boost::chrono::nanoseconds(1));
				if (level == 0)
					res.emplace_back(Node<size_type>{origin, size});
				return VisitorAction::continue_;
			}
		);
		if (i >= MAX_ITER)
			ETC_LOG.warn("Some nodes are ignored (too much iterations)");
		if (res.size() >= MAX_NODES)
			ETC_LOG.warn("Some nodes are ignored (too much results)");
		ETC_LOG.debug("Found", res.size(), "nodes in", i, "iterations");
		return res;
	}

	template<typename size_type>
	std::vector<Node<size_type>>
	find_close_nodes(Node<size_type> const& pos)
	{
		std::vector<Node<size_type>> res;
		typedef typename Tree<size_type>::vector_type vector_type;
		auto const child_size = pos.size;
		static const int size = 2;
		for (int x = -size; x <= size; ++x)
			for (int y = -size; y <= size; ++y)
				for (int z = -size; z <= size; ++z)
					res.emplace_back(pos.origin + vector_type{x, y, z} * child_size, child_size);
		for (int x = -size - 2; x < size + 3; ++x)
		{
			if (x >= -size && x <= size) continue;
			for (int y = -size -2; y < size + 3; ++y)
			{
				if (y >= -size && y <= size) continue;
				for (int z = -size - 2; z < size + 3; ++z)
				{
					if (z >= -size && z <= size) continue;
					res.emplace_back(pos.origin + vector_type{x, y, z} * child_size * child_size, child_size * child_size);
				}
			}
		}
		return res;
	}

	template
	CUBEAPP_API std::vector<Node<int64_t>>
	find_nodes(Tree<int64_t> const& tree,
	           cube::gl::vector::Vector3d const& pos,
	           cube::gl::frustum::Frustumd const& frustum) ETC_NOEXCEPT;

	template
	CUBEAPP_API
	std::vector<Node<int64_t>>
	find_close_nodes(Node<int64_t> const& pos);

	template<typename size_type>
	std::ostream& operator <<(std::ostream& out, Node<size_type> const& node)
	{
		return out
			<< "<Node origin=" << node.origin
			<< " size=" << node.size
			<< ">";
	}

	template
	CUBEAPP_API
	std::ostream& operator <<(std::ostream& out, Node<int64_t> const& node);

	template struct CUBEAPP_API Node<int64_t>;

}}}
