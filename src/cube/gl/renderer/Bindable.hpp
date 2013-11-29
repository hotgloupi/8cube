#ifndef  CUBE_GL_RENDERER_BINDABLE_HPP
# define CUBE_GL_RENDERER_BINDABLE_HPP

# include "fwd.hpp"

# include <etc/types.hpp>

namespace cube { namespace gl { namespace renderer {

	class CUBE_API Bindable
	{
	private:
		etc::size_type _bound;

	public:
		Bindable() noexcept
			: _bound{0}
		{}

		virtual
		~Bindable()
		{}

	protected:
		/**
		 * @brief Bind the bindable with the current state.
		 *
		 * This method is called when the bindable *needs* to be bound.
		 */
		virtual
		void _bind(State const& state) = 0;

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
		void _unbind() noexcept = 0;

	public:
		/**
		 * @brief Check is the bindable is bound.
		 */
		inline
		etc::size_type bound() const noexcept
		{ return _bound; }

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
		enum InternalMethod { internal_method };
		inline
		void _bind(State const& state, InternalMethod)
		{ if (++_bound == 1) _bind(state); }

		inline
		void _unbind(InternalMethod) noexcept
		{ if (--_bound == 0) _unbind(); }

	protected:
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

		Guard(Bindable& bindable, State const& state)
			: bindable(bindable)
		{ this->bindable._bind(state, Bindable::internal_method); }

	public:
		~Guard()
		{ this->bindable._unbind(Bindable::internal_method); }
		friend class Painter;
	};

	template<typename T>
	struct Bindable::InternalGuard
	{
	private:
		T* _bindable;

	public:
		InternalGuard(T& bindable)
			: _bindable(!bindable.bound() ? &bindable : nullptr)
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
