#ifndef  CUBE_GL_RENDERER_PAINTER_HPP
# define CUBE_GL_RENDERER_PAINTER_HPP

# include "fwd.hpp"
# include "Bindable.hpp"

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
		std::set<BindableBase*> _bound_drawables;

	public:
		Painter(Painter&& other);
		~Painter();

		/**
		 * @brief Bind a drawable to the painter.
		 */
		template<typename... Args>
		void bind(Bindable<Args...>& bindable, Args&... args)
		{
			bindable.__bind(args...);
			_bound_drawables.insert(&bindable);
			_update_parameters(bindable);
		}

		/**
		 * @brief Proxy of a painter.
		 *
		 */
		struct Proxy;

		/**
		 * @brief Bind all bindable and return a painter proxy.
		 */
		template<typename... T>
		Proxy with(T&... bindables);

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
		 * @brief Draw a drawable.
		 */
		template<typename... Args>
		void draw(Drawable<Args...>& drawable,
		          Args&... args)
		{
			drawable._draw(*this, args...);
		}

		/**
		 * @brief Manually unbind any bindable.
		 *
		 * This is usually done automatically when the painter goes out of
		 * scope.
		 */
		void unbind(BindableBase& bindable);

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

	private:
		// used internally to set all parameters
		void _update_parameters(BindableBase& bindable);

		// used by the renderer begin method
	private:
		Painter(Renderer& renderer);
		friend class Renderer;
	};

	struct Painter::Proxy
	{
		ETC_LOG_COMPONENT("cube.gl.renderer.Painter.Proxy");
	private:
		Painter&                    _self;
		etc::size_type              _guards_size;
		BindableBase::GuardBase*    _guards;

	private:
		BindableBase::GuardBase*
		_new_guard_mem();

		template<typename... Args, typename... Tail>
		void _init(Bindable<Args...>& first,
		           Args&... args,
		           Tail&... tail)
		{
			static_assert(
				sizeof(BindableBase::GuardBase) == sizeof(Bindable<Args...>::Guard),
				"Derived guard has to have the same size."
			);
			ETC_TRACE.debug("Insert guard of", &first);
			typedef typename Bindable<Args...>::Guard Guard;
			new (_new_guard_mem()) Guard(first, args...);
			_init(tail...);
		}

		// termination
		void _init();

	public:
		template<typename... T>
		explicit
		Proxy(Painter& self, T&... drawables)
			: _self(self)
			, _guards_size{0}
			, _guards{nullptr}
		{
			_init(drawables...);
		}

		Proxy(Proxy&& other);
		~Proxy();

		inline
		Painter* operator ->()
		{
			return &_self;
		}
	};

	template<typename... T>
	Painter::Proxy
	Painter::with(T&... drawables)
	{
		return Proxy(*this, drawables...);
	}

}}}

#endif


