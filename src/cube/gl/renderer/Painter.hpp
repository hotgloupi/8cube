#ifndef  CUBE_GL_RENDERER_PAINTER_HPP
# define CUBE_GL_RENDERER_PAINTER_HPP

# include "fwd.hpp"
# include "Bindable.hpp"
# include "Exception.hpp"

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
	class Painter
		: private boost::noncopyable
	{
	private:
		Renderer&               _renderer;
		State&                  _current_state;
		std::set<Bindable*>     _bound_drawables;

	public:
		Painter(Painter&& other);
		~Painter();

		inline
		State& state()
		{ return _current_state; }

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
		 * @brief Draw arrays.
		 */
		void draw_arrays(DrawMode mode,
		                 VertexBuffer& vertices,
		                 etc::size_type start = 0,
		                 etc::size_type count = -1);

		/**
		 * Send to bound drawable (of this painter) a new matrix.
		 */
		void update(MatrixKind kind, matrix_type const& matrix);

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

	private:
		// used by the renderer begin method
		Painter(Renderer& renderer);
		friend class Renderer;

	private:
		// For the proxy
		void _new_guard(Bindable::Guard* mem, Bindable& bindable)
		{
			if (_bound_drawables.find(&bindable) != _bound_drawables.end())
			{
				throw Exception{
					"The bindable is already bound to this painter"
				};
			}
			new (mem) Bindable::Guard(bindable, _current_state);
			_bound_drawables.insert(&bindable);
		}

		void _delete_guard(Bindable::Guard& guard)
		{
			auto it = _bound_drawables.find(&guard.bindable);
			// We assert here since we know that a bindable cannot be bound
			// twice to the same painter.
			assert(it != _bound_drawables.end());
			_bound_drawables.erase(it);
			guard.~Guard();
		}

	};

	template<etc::size_type const bounds>
	struct Painter::Proxy
	{
	private:
		Painter&    _self;
		uint8_t     _guards[sizeof(Bindable::Guard) * bounds];

	public:
		template<typename... Args>
		explicit
		Proxy(Painter& self, Args&&... bindables)
			: _self(self)
			, _guards{}
		{
			_init(
				(Bindable::Guard*) _guards,
				std::forward<Args>(bindables)...
			);
		}

		~Proxy()
		{
			auto tab = (Bindable::Guard*) _guards;
			for (etc::size_type i = 0; i < bounds; ++i)
			{
				_self._delete_guard(tab[i]);
			}
		}
		Proxy(Proxy&& other) = default;

		inline
		Painter* operator ->()
		{
			return &_self;
		}

	private:
		Proxy(Proxy const&) = delete;
		Proxy& operator =(Proxy const&) = delete;
		Proxy& operator =(Proxy&&) = delete;

	private:
		template<typename T>
		struct ERROR_HELPER_incomplete_type_detected_for
		{
			static const int size = sizeof(T);
		};
	private:
		template<typename First, typename... Tail>
		inline
		void _init(Bindable::Guard* mem,
		           First&& first,
		           Tail&&... tail)
		{
			static_assert(
				ERROR_HELPER_incomplete_type_detected_for<First>::size > 0,
				"Incomplete type detected!"
			);
			_self._new_guard(mem, first);
			try {
				_init(mem + 1, std::forward<Tail>(tail)...);
			} catch (...) {
				_self._delete_guard(*mem);
				throw;
			}
		}

		// termination
		inline void _init(Bindable::Guard*) {}
	};

	template<typename... Args>
	Painter::Proxy<sizeof...(Args)>
	Painter::with(Args&&... drawables)
	{
		return Proxy<sizeof...(Args)>{
			*this,
			std::forward<Args>(drawables)...
		};
	}

}}}

#endif


