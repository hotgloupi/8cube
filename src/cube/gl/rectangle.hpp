#ifndef  CUBE_GL_RECTANGLE_HPP
# define CUBE_GL_RECTANGLE_HPP

# include "vector.hpp"

# include <etc/compiler.hpp>

namespace cube { namespace gl { namespace rectangle {

	template<typename T>
	struct Rectangle2
	{
	public:
		union { struct { T x, y; }; T _position[2]; };
		union { struct { T w, h; }; T _size[2]; };

	public:
		inline
		Rectangle2(T x, T y, T w, T h) ETC_NOEXCEPT
			: x{x}
			, y{y}
			, w{w}
			, h{h}
		{}

		inline
		Rectangle2(Rectangle2 const&) = default;
		inline
		Rectangle2& operator =(Rectangle2 const&) = default;
	};

	typedef Rectangle2<float> Rectangle2f;

}}} // !cube::gl::rectangle

#endif

