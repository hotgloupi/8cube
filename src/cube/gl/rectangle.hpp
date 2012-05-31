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
			cube::gl::vector::Vector2<T> position;
			T x, y;
		};

		union
		{
			cube::gl::vector::Vector2<T> size;
			T w, h;
		};

	public:
		Rectangle2(T x, T y, T w, T h)
			: position(x, y)
			, size(w, h)
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

