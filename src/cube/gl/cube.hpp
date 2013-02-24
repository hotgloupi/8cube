#ifndef  CUBE_GL_CUBE_HPP
# define CUBE_GL_CUBE_HPP

# include "vector.hpp"

# include <etc/meta/if.hpp>

# include <iosfwd>

namespace cube { namespace gl { namespace cube {

	enum class XAxis { right = 1,  left = -1 };
	enum class YAxis { top = 1,    down = -1 };
	enum class ZAxis { front = 1,  back = -1 };

	template<typename T>
	struct Cube3
	{
	public:
		typedef vector::Vector3<T>  vec3;

	public:
		vec3    center;
		T       width;

	public:
		Cube3(vec3 const& center, T const width)
			: center{center}
			, width{width}
		{}
		Cube3(Cube3 const&) = default;
		Cube3(Cube3&&) = default;
		Cube3& operator =(Cube3 const&) = default;
		Cube3& operator =(Cube3&&) = default;

	public:
		inline
		vec3 vertex(XAxis const x, YAxis const y, ZAxis const z) const
		{
			(void) x, (void) y, (void) z; // strange unused args bug...
			T const r = width / static_cast<T>(2);
			return vec3{
				center.x + static_cast<T>(x) * r,
				center.y + static_cast<T>(y) * r,
				center.z + static_cast<T>(z) * r,
			};
		}

		inline
		vec3 vertex(etc::size_type const index) const
		{
			static int const map[] = {
				(int)XAxis::left,  (int)YAxis::top,  (int)ZAxis::front,
				(int)XAxis::left,  (int)YAxis::top,  (int)ZAxis::back,
				(int)XAxis::left,  (int)YAxis::down, (int)ZAxis::front,
				(int)XAxis::left,  (int)YAxis::down, (int)ZAxis::back,
				(int)XAxis::right, (int)YAxis::top,  (int)ZAxis::front,
				(int)XAxis::right, (int)YAxis::top,  (int)ZAxis::back,
				(int)XAxis::right, (int)YAxis::down, (int)ZAxis::front,
				(int)XAxis::right, (int)YAxis::down, (int)ZAxis::back,
			};
			int const* v = &map[(index % 8) * 3];
			return this->vertex(
				(XAxis) *v,
				(YAxis) *(v + 1),
				(ZAxis) *(v + 2)
			);
		}

		inline
		bool contains(T const x, T const y, T const z) const
		{
			return this->contains(vec3{x, y, z});
		}

		inline
		bool contains(vec3 const& pos) const
		{
			typedef typename etc::meta::if_<
			      std::is_integral<T>::value
				, std::make_signed<T>
				, std::enable_if<true, T>
			>::type::type signed_t;
			signed_t const r = this->width / static_cast<signed_t>(2);
			return !(
				    glm::abs(((signed_t) pos.x) - ((signed_t) this->center.x)) > r
				||  glm::abs(((signed_t) pos.y) - ((signed_t) this->center.y)) > r
				||  glm::abs(((signed_t) pos.z) - ((signed_t) this->center.z)) > r
			);
		}
	};

	typedef Cube3<float>    Cube3f;
	typedef Cube3<double>   Cube3d;
	typedef Cube3<int32_t>  Cube3i;
	typedef Cube3<uint32_t> Cube3u;
	typedef Cube3<int64_t>  Cube3il;
	typedef Cube3<uint64_t> Cube3ul;

	template<typename T>
	std::ostream& operator <<(std::ostream& out, Cube3<T> const& cube);

}}}

#endif

