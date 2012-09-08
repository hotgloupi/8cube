#ifndef  CUBE_GL_RENDERER_PAINTER_HPP
# define CUBE_GL_RENDERER_PAINTER_HPP

# include "fwd.hpp"

# include <boost/noncopyable.hpp>

# include <set>

namespace cube { namespace gl { namespace renderer {

	/**
	 * @Brief a Painter is a temporary class meant to bind/unbind drawables.
	 *
	 * This is an abstract class that should be implemented per renderer basis.
	 */
	struct Painter
		: private boost::noncopyable
	{
	private:
		Renderer&               _renderer;
		State&                  _current_state;
		std::set<Drawable*>     _bound_drawables;

	public:
		Painter(Painter&& other);
		~Painter();

		/**
		 * @brief Bind a drawable to the painter.
		 */
		void bind(Drawable& drawable);

		/**
		 * @brief Send indices to the rendering system.
		 */
		void draw_elements(DrawMode mode,
		                   VertexBuffer& indices,
		                   unsigned int start = 0,
		                   unsigned int count = -1);
		/**
		 * @brief Manually unbind any drawable.
		 *
		 * This is usually done automatically when the painter goes out of
		 * scope.
		 */
		void unbind(Drawable& drawable);

		/**
		 * Send to bound drawable (of this painter) a new matrix.
		 */
		void update(MatrixKind kind, matrix_type const& matrix);

		/**
		 * Convert painter to a boolean. If the painter is not available for
		 * renderering, this should convert to false.
		 *
		 * Default implementation always convert to true.
		 * ------------------------------------------------
		 *  if (auto painter = renderer.begin(Mode::_2d))
		 *  {
		 *      // Do rendering.
		 *  }
		 */
		operator bool() const
		{ return true; }

		// used by the renderer begin method
	private:
		Painter(Renderer& renderer);
		friend class Renderer;
	};


}}}

#endif


