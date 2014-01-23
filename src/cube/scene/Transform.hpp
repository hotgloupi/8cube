#ifndef CUBE_SCENE_TRANSFORM_HPP
# define CUBE_SCENE_TRANSFORM_HPP

# include "GroupNode.hpp"

# include <cube/gl/matrix.hpp>

namespace cube { namespace scene {

	class Transform
		: public GroupNode
		, public VisitableNode<Transform>
	{
	public:
		typedef gl::matrix::mat4f matrix_type;

	private:
		matrix_type _transformation;

	public:
		explicit Transform(Graph& graph,
		                   std::string name,
		                   matrix_type const& transformation)
			: GroupNode{graph, std::move(name)}
			, _transformation(transformation)
		{}
	public:
		using VisitableNode<Transform>::visit;
	};

}}

#endif
