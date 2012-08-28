#ifndef  CUBE_GL_RENDERER_PAINTER_HPP
# define CUBE_GL_RENDERER_PAINTER_HPP

# include "fwd.hpp"

# include <boost/noncopyable.hpp>

# include <set>
# include <vector>

namespace cube { namespace gl { namespace renderer {

	///////////////////////////////////////////////////////////////////////////
	// Painter class

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
		void draw(std::vector<Drawable*> objects);
		void bind(Drawable& drawable);
		void draw_elements(DrawMode mode,
		                   VertexBuffer& indices,
		                   unsigned int start = 0,
		                   unsigned int count = -1);
		void unbind(Drawable& drawable);
		void update(MatrixKind kind, matrix_type const& matrix);

		// used by the renderer begin method
	private:
		Painter(Renderer& renderer);
		friend class Renderer;
	};


}}}

#endif


