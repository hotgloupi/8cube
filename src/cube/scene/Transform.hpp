#ifndef CUBE_SCENE_TRANSFORM_HPP
# define CUBE_SCENE_TRANSFORM_HPP

# include "Node.hpp"

# include <cube/gl/matrix.hpp>
# include <cube/gl/vector.hpp>

namespace cube { namespace scene {

	class Transform
		: public Node
		, public VisitableNode<Transform>
	{
	public:
		typedef gl::matrix::mat4f matrix_type;
		typedef gl::vector::vec3f vector_type;

	private:
		matrix_type _transformation;

	public:
		explicit Transform(std::string name,
		                   matrix_type const& transformation)
			: Node{std::move(name)}
			, _transformation(transformation)
		{}

	public:
		inline
		matrix_type const& value() const { return _transformation; }

		void translate(vector_type const& v)
		{ _transformation = gl::matrix::translate(_transformation, v); }

	public:
		using VisitableNode<Transform>::visit;
	};

}}

#endif
