#ifndef  CUBE_GL_RECTANGLE_HPP
# define CUBE_GL_RECTANGLE_HPP

# include "vector.hpp"

# include <boost/config.hpp>

namespace cube { namespace gl { namespace rectangle {

	template<typename T>
	struct Rectangle2
	{
	public:
		union
		{
#ifndef BOOST_MSVC
			vector::Vector2<T> position;
#else
			struct __PositionType { T x, y; } position;
#endif
			struct { T x, y; };
		};

		union
		{
#ifndef BOOST_MSVC
			vector::Vector2<T> size;
#else
			struct __SizeType { T x, y; } size;
#endif
			struct { T w, h; };
		};

	public:
		Rectangle2(T x, T y, T w, T h)
			: x{x}
			, y{y}
			, w{w}
			, h{h}
		{}

		Rectangle2& operator =(Rectangle2 const& other)
		{
			if (this != &other)
			{
				this->position = other.position;
				this->size = other.size;
			}
			return *this;
		}
	};

	typedef Rectangle2<float> Rectangle2f;

}}} // !cube::gl::rectangle

#endif

