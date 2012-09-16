#ifndef  CUBE_GL_RENDERER_DRAWABLE_HPP
# define CUBE_GL_RENDERER_DRAWABLE_HPP

# include "fwd.hpp"

namespace cube { namespace gl { namespace renderer {

	/**
	 * @brief Represent a drawable object.
	 */
	template<typename... Args>
	class Drawable
	{
	protected:
		/**
		 * Draw the object.
		 */
		virtual
		void _draw(Painter& painter, Args&... args) = 0;


		// Only a painter can call _draw(...)
		friend Painter;

	public:
		virtual
		~Drawable() {}
	};

}}}

#endif
