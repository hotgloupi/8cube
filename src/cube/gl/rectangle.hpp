#ifndef  CUBE_GL_RECTANGLE_HPP
# define CUBE_GL_RECTANGLE_HPP

# include "vector.hpp"

# include <cube/gl/fwd.hpp>

# include <etc/compiler.hpp>

namespace cube { namespace gl { namespace rectangle {

	template<typename T>
	struct Rectangle
	{
	public:
		union { struct { T x, y; }; T _position[2]; };
		union { struct { T w, h; }; T _size[2]; };

	public:
		inline
		Rectangle(T x, T y, T w, T h) ETC_NOEXCEPT
			: x{x}
			, y{y}
			, w{w}
			, h{h}
		{}

		inline
		Rectangle(Rectangle const&) = default;
		inline
		Rectangle& operator =(Rectangle const&) = default;
	};

}}} // !cube::gl::rectangle

#endif

