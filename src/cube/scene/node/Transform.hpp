#ifndef  CUBE_SCENE_NODE_TRANSFORM_HPP
# define CUBE_SCENE_NODE_TRANSFORM_HPP

# include "Node.hpp"

# include <cube/gl/matrix.hpp>
# include <cube/gl/vector.hpp>
# include <cube/units/angle.hpp>

namespace cube { namespace scene { namespace node {

	class Transform
		: public Node
		, public Visitable<Transform>
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

		void value(matrix_type const& mat)
		{ _transformation = mat; }

		void translate(vector_type const& v)
		{ _transformation = gl::matrix::translate(_transformation, v); }

		void rotate(units::Angle angle, vector_type const& v)
		{ _transformation = gl::matrix::rotate(_transformation, angle, v); }

	public:
		using Visitable<Transform>::visit;
	};

}}}

#endif
