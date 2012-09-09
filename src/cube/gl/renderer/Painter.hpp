#ifndef  CUBE_GL_RENDERER_PAINTER_HPP
# define CUBE_GL_RENDERER_PAINTER_HPP

# include "fwd.hpp"
# include "Drawable.hpp"

# include <etc/types.hpp>
# include <etc/log.hpp>

# include <boost/noncopyable.hpp>

# include <set>
# include <vector>

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

		struct PainterProxy
		{
			ETC_LOG_COMPONENT("cube.gl.renderer.Painter.PainterProxy");
		private:
			Painter&                            _self;
			std::vector<Drawable::BindGuard>    _guards;

			void _init() {}
			template<typename... T>
			void _init(Drawable& first, T&... tail)
			{
				_guards.emplace_back(first);
				_init(tail...);
			}
		public:
			template<typename... T>
			PainterProxy(Painter& self, T&... drawables)
				: _self(self)
			{
				_init(drawables...);
				ETC_TRACE.debug("Painter proxy initialized");
			}
			PainterProxy(PainterProxy&& other)
				: _self(other._self)
				, _guards(std::move(other._guards))
			{}
			Painter* operator ->()
			{
				return &_self;
			}
		};

		template<typename... T>
		PainterProxy with(T&... drawables)
		{
			return PainterProxy(*this, drawables...);
		}

		/**
		 * @brief Send indices to the rendering system.
		 *
		 * @param mode      Primitive kind to be drawn
		 * @param indices   indices array
		 * @param start     indice offset
		 * @param count     the number of indice to draw.
		 *
		 * The index buffer is bound by this function.
		 * Parameters @a start and @a count are optional, their default value
		 * will conviniently draw all contained indices.
		 */
		void draw_elements(DrawMode mode,
		                   VertexBuffer& indices,
		                   unsigned int start = 0,
		                   unsigned int count = -1);

		/**
		 * @brief Draw arrays.
		 */
		void draw_arrays(DrawMode mode,
		                 VertexBuffer& vertices,
		                 etc::size_type start = 0,
		                 etc::size_type count = -1);

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


