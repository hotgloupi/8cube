#ifndef  CUBE_GL_RECTANGLE_HPP
# define CUBE_GL_RECTANGLE_HPP

# include "vector.hpp"

namespace cube { namespace gl { namespace rectangle {

	template<typename T>
	struct Rectangle2
	{
	public:
		union
		{
			vector::Vector2<T> position;
			struct { T x, y; };
		};

		union
		{
			vector::Vector2<T> size;
			struct { T w, h; };
		};

	public:
		inline
		Rectangle2(T x, T y, T w, T h) noexcept
			: position{x, y}
			, size{w, h}
		{}

		inline
		Rectangle2(Rectangle2 const& other) noexcept
			: position{other.position}
			, size{other.size}
		{}

		inline
		Rectangle2& operator =(Rectangle2 const& other) noexcept
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

