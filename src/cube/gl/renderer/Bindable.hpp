#ifndef  CUBE_GL_RENDERER_BINDABLE_HPP
# define CUBE_GL_RENDERER_BINDABLE_HPP

# include "fwd.hpp"

# include <etc/types.hpp>

namespace cube { namespace gl { namespace renderer {

	class Bindable
	{
	private:
		etc::size_type __bound;

	public:
		Bindable()
			: __bound{0}
		{}

		virtual
		~Bindable()
		{}

	protected:
		/**
		 * @brief   Unbind the bindable.
		 *
		 * This method is called by the painter only if the bindable actually
		 * *needs* to be unbound. That means implementations of this method do
		 * not need to maintain or check a bound state.
		 *
		 * @warning This method should only be used in the subclasses
		 *          constructors. If you need to bind/unbind in a subclass
		 *          method, use the Bindable<>::Guard class.
		 */
		virtual
		void _unbind() = 0;

		/**
		 * @brief Bind the bindable with the current state.
		 *
		 * This method is called when the bindable *needs* to be bound.
		 */
		virtual
		void _bind(State const& state) = 0;

	public:
		/**
		 * @brief Check is the bindable is bound.
		 */
		inline
		etc::size_type bound() const
		{ return __bound; }

	protected:
		/**
		 * @brief A generic guard that does not affect the bound count.
		 *
		 * The class is meant to be used internally by implementation whenever
		 * they need to ensure the resource they hold is bound.
		 *
		 * To do so, final class has to make `struct InternalGuard<FinalClass>'
		 * a friend and implement the method `FinalClass::_bind()'.
		 */
		template<typename T>
		struct InternalGuard;

	private:
		inline
		void __unbind()
		{
			if (__bound > 0)
				__bound -= 1;
			if (__bound == 0)
				_unbind();
		}

		inline
		void __bind(State const& state)
		{
			__bound += 1;
			if (__bound == 1)
				_bind(state);
		}

		/**
		 * @brief Bind guard used by the painter.
		 *
		 * The guard increment the bound counter at construction and
		 * decrement it at destruction.
		 */
		struct Guard;
		friend struct Guard;
		friend class Painter;
	};

	struct Bindable::Guard
	{
	public:
		Bindable& bindable;

	protected:
		Guard(Bindable& bindable, State const& state)
			: bindable(bindable)
		{ this->bindable.__bind(state); }

	public:
		~Guard()
		{ this->bindable.__unbind(); }
		friend class Painter;
	};

	template<typename T>
	struct Bindable::InternalGuard
	{
	private:
		T* _bindable;

	public:
		InternalGuard(T& bindable)
			: _bindable(bindable.bound() ? &bindable : nullptr)
		{
			if (_bindable != nullptr)
				_bindable->_bind();
		}
		~InternalGuard()
		{
			if (_bindable != nullptr)
				_bindable->_unbind();
		}
	};

}}}

#endif
