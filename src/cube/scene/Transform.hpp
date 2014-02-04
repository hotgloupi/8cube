#ifndef CUBE_SCENE_TRANSFORM_HPP
# define CUBE_SCENE_TRANSFORM_HPP

# include "Node.hpp"

# include <cube/gl/matrix.hpp>

namespace cube { namespace scene {

	class Transform
		: public Node
		, public VisitableNode<Transform>
	{
	public:
		typedef gl::matrix::mat4f matrix_type;

	private:
		matrix_type _transformation;

	public:
		explicit Transform(std::string name,
		                   matrix_type const& transformation)
			: Node{std::move(name)}
			, _transformation(transformation)
		{}

	public:
		using VisitableNode<Transform>::visit;
	};

}}

#endif
