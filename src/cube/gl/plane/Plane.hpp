#ifndef  CUBE_GL_PLANE_PLANE_HPP
# define CUBE_GL_PLANE_PLANE_HPP

# include <cube/api.hpp>
# include <cube/gl/vector.hpp>

# include <etc/compiler.hpp>

# include <iosfwd>

namespace cube { namespace gl { namespace plane {

	/**
	 * Represent a plane in a 3d space coordinate.
	 *
	 * The plane is not normalized at construction, but the `magnitude` is
	 * computed and used to normalize distance. This is the only correct way to
	 * work with integer coordinates.
	 *
	 * The plane is oriented by its normal. When created from three points,
	 * the orientation is a vector normal to the anti-clockwise face.
	 *
	 *       ^
	 *    B  |  A
	 *       C
	 *
	 * @note that the magnitude is never zero, even for a null plane.
	 */
	template<typename T>
	struct CUBE_API Plane
	{
	public:
		typedef vector::Vector3<T>      vec3;
		typedef vector::Vector4<T>      vec4;
		typedef vector::Vector4<double> vec4d;

	private:
		vec4d   _coef;

	public:
		/**
		 * @brief Construct a null plane.
		 */
		Plane() ETC_NOEXCEPT
			: _coef{}
		{}

		/**
		 * @brief Construct a plane from 3 points.
		 */
		Plane(vec3 const& p0,
		      vec3 const& p1,
		      vec3 const& p2) ETC_NOEXCEPT;

		/**
		 * @brief Construct a plane from a point and a normal.
		 */
		Plane(vec3 const& point, vec3 const& normal) ETC_NOEXCEPT;

		/**
		 * @brief Retreive plane equation coefficients.
		 */
		inline
		vec4d const& coef() const ETC_NOEXCEPT
		{ return _coef; }

		/**
		 * @brief Retreive a normal of this plane
		 */
		inline
		vec3 normal() const ETC_NOEXCEPT
		{ return vec3{_coef[0], _coef[1], _coef[2]}; }

		/**
		 * @brief Compute the algebraic distance to a point.
		 */
		inline
		double distance(vec3 const& p) const ETC_NOEXCEPT
		{
			return (
				p.x * _coef[0] + p.y * _coef[1] + p.z * _coef[2] + _coef[3]
			);
		}
	};

	typedef Plane<float>    Planef;
	typedef Plane<double>   Planed;
	//typedef Plane<int32_t>  Planei;
	//typedef Plane<uint32_t> Planeu;
	//typedef Plane<int64_t>  Planeil;
	//typedef Plane<uint64_t> Planeul;

	template<typename T>
	CUBE_API
	std::ostream& operator <<(std::ostream& out, Plane<T> const& plane);

}}}

#endif
