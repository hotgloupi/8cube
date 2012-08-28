#ifndef  CUBE_GL_RENDERER_DRAWABLE_HPP
# define CUBE_GL_RENDERER_DRAWABLE_HPP

# include "fwd.hpp"

# include "Exception.hpp"

# include <boost/noncopyable.hpp>

namespace cube { namespace gl { namespace renderer {

	class Drawable
		: private boost::noncopyable
	{
	public:
		Drawable() : __bound{false} {}
		virtual ~Drawable() {}

		virtual
		void update(MatrixKind, matrix_type const&) {}

	protected:
		virtual
		void _bind() = 0;

		virtual
		void _unbind() = 0;

	private:
		friend class Painter;
		bool __bound;
		void __bind()
		{
			if (this->__bound)
				throw Exception{"Already bound drawable."};
			_bind();
			__bound = true;
		}
		void __unbind()
		{
			if (!this->__bound)
				throw Exception{"Not bound drawable."};
			_unbind();
			__bound = false;
		}
	};

}}}

#endif

