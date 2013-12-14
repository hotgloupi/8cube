#ifndef  CUBE_GL_RENDERER_PAINTER_HPP
# define CUBE_GL_RENDERER_PAINTER_HPP

# include "fwd.hpp"
# include "Bindable.hpp"
# include "Drawable.hpp"
# include "Exception.hpp"

# include <etc/compiler.hpp>
# include <etc/log.hpp>
# include <etc/types.hpp>

# include <boost/noncopyable.hpp>

# include <set>
# include <array>

namespace cube { namespace gl { namespace renderer {

	/**
	 * @Brief a Painter is a temporary class meant to bind/unbind drawables.
	 *
	 * This is an abstract class that should be implemented per renderer basis.
	 */
	class CUBE_API Painter
		: private boost::noncopyable
	{
	private:
		Renderer&               _renderer;
		etc::size_type          _state_count;
		std::set<Bindable*>     _bound_drawables;

	public:
		Painter(Painter&& other);
		~Painter();

	private:
		// used by the renderer begin method
		Painter(Renderer& renderer);
		friend class Renderer;

	public:
		std::weak_ptr<State> state();

		/**
		 * Push a new state bound to this painter. When the painter is
		 * destroyed all pushed states are poped, but you can call pop_state()
		 * if you want to to handle it yourself.
		 *
		 * The returned new state is an exact copy of the current one.
		 */
		std::weak_ptr<State> push_state();

		/**
		 * Pop a previously pushed state. If there is no more state to pop, an
		 * exception will be thrown.
		 */
		void pop_state();

		/**
		 * @brief Proxy of a painter.
		 *
		 */
		template<etc::size_type const bounds>
		struct Proxy;
		template<etc::size_type const bounds>
		friend struct Proxy;

		/**
		 * @brief Bind all bindable and return a painter proxy.
		 */
		template<typename... Args>
		Proxy<sizeof...(Args)> with(Args&&... bindables);

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
		                   etc::size_type start = 0,
		                   etc::size_type count = -1);

		/**
		 * @brief Draw a variadic number of drawables.
		 */
		template<typename First, typename... Args>
		Painter& draw(First&& view, Args&&... args)
		{
			this->_draw_dispatch(std::forward<First>(view));
			return this->draw(std::forward<Args>(args)...);
		}

	private:
		Painter& draw() { return *this; }
		void _draw_dispatch(Drawable& view) { view._draw(*this); }
		void _draw_dispatch(DrawablePtr& view) { if (view) view->_draw(*this); }
		void _draw_dispatch(Drawable* view) { if (view) view->_draw(*this); }
		template<typename Iterable>
		void _draw_dispatch(Iterable&& drawables)
		{
			for (auto& view: drawables)
				this->_draw_dispatch(view);
		}

	public:
		/**
		 * @brief Draw arrays.
		 */
		void draw_arrays(DrawMode mode,
		                 VertexBuffer& vertices,
		                 etc::size_type start = 0,
		                 etc::size_type count = -1);

		/**
		 * Convert painter to a boolean. If the painter is not ready for
		 * renderering, this should convert to false. It meant to be used in a
		 * if statement.
		 *
		 * Default implementation always convert to true.
		 * ------------------------------------------------
		 *  if (auto painter = renderer.begin(Mode::_2d))
		 *  {
		 *      painter.with(vbo, shader)->draw_elements(...);
		 *  }
		 */
		inline operator bool() const { return true; }
	};

	template<etc::size_type const bounds>
	struct Painter::Proxy
	{
	private:
		Painter&                            _self;
		std::array<Bindable::Guard, bounds> _guards;

	public:
		template<typename... Args>
		explicit
		Proxy(Painter& self, Args&&... bindables)
			: _self(self)
			, _guards{{
				Bindable::Guard{
					std::forward<Args>(bindables),
					self.state().lock(),
				}...
			}}
		{}

		inline
		Proxy(Proxy&& other) ETC_NOEXCEPT
			: _self(other._self)
			, _guards(std::move(other._guards))
		{}

		~Proxy()
		{}

		inline
		Painter* operator ->() ETC_NOEXCEPT
		{ return &_self; }

	private:
		Proxy(Proxy const&) = delete;
		Proxy& operator =(Proxy const&) = delete;
		Proxy& operator =(Proxy&&) = delete;
	};

	template<typename... Args>
	Painter::Proxy<sizeof...(Args)>
	Painter::with(Args&&... drawables)
	{
		return Proxy<sizeof...(Args)>(
			*this,
			std::forward<Args>(drawables)...
			);
	}

}}}

#endif
