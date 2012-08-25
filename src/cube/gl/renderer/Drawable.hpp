#ifndef  CUBE_GL_RENDERER_DRAWABLE_HPP
# define CUBE_GL_RENDERER_DRAWABLE_HPP

# include "fwd.hpp"

namespace cube { namespace gl { namespace renderer {

	class Drawable
	{
	public:
		virtual ~Drawable() {}

		virtual
		void update(MatrixKind, matrix_type const&) {}

	protected:
		virtual
		void _bind() = 0;

		virtual
		void _unbind() = 0;

		friend class Painter;
	};

}}}

#endif

