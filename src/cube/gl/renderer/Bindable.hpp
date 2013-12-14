#ifndef  CUBE_GL_RENDERER_BINDABLE_HPP
# define CUBE_GL_RENDERER_BINDABLE_HPP

# include "fwd.hpp"

# include <etc/compiler.hpp>
# include <etc/types.hpp>

namespace cube { namespace gl { namespace renderer {

	class CUBE_API Bindable
	{
	private:
		etc::size_type         _bound;
		std::shared_ptr<State> _bound_state;

	public:
		Bindable() ETC_NOEXCEPT;
		virtual ~Bindable();

	protected:
		/**
		 * @brief Bind the bindable with the current state.
		 *
		 * This method is called when the bindable *needs* to be bound. When
		 * called the method bound() returns a number greater than 0, and
		 * bound_state() a valid state.
		 */
		virtual
		void _bind() = 0;

		/**
		 * @brief   Unbind the bindable.
		 *
		 * This method is called by the painter only if the bindable actually
		 * *needs* to be unbound. That means implementations of this method do
		 * not need to maintain or check the bound state. When called, the
		 * method bound() returns 0 and bound_state() returns a valid state.
		 *
		 * @warning This method should only be used directly in the subclasses
		 *          constructors. If you need to bind/unbind in a subclass
		 *          method, use the Bindable<>::Guard class.
		 */
		virtual
		void _unbind() ETC_NOEXCEPT = 0;

	public:
		/**
		 * @brief Check is the bindable is bound.
		 */
		inline
		etc::size_type bound() const ETC_NOEXCEPT
		{ return _bound; }

		/**
		 * Returns the bound state.
		 */
		State& bound_state();
		inline std::shared_ptr<State> const& shared_state()
		{ return _bound_state; }

	public:
		/**
		 * @brief Bind guard.
		 *
		 * The guard increment the bound counter at construction and decrement
		 * it at destruction. During the guard lifetime, bound() returns a
		 * number greater that 0, and bound_state() returns a valid state.
		 */
		struct Guard;
		friend struct Guard;

	protected:
		/**
		 * @brief A generic guard that does not affect the bound count.
		 *
		 * The class is meant to be used internally by implementation whenever
		 * they need to ensure the resource they hold is bound.
		 *
		 * To do so, final class has to make `struct InternalGuard<FinalClass>'
		 * a friend.
		 *
		 * @note You should not use bound() or bound_state() methods when using
		 * this guard.
		 */
		template<typename T>
		struct InternalGuard;

	private:
		enum InternalMethod { internal_method };
		void _bind(InternalMethod);

		void _unbind(InternalMethod) ETC_NOEXCEPT;
	};

	struct CUBE_API Bindable::Guard
	{
	private:
		Bindable* _bindable;

	public:
		Guard(Bindable& bindable, std::shared_ptr<State> const& state);
		inline
		Guard(Bindable& bindable, std::weak_ptr<State> const& state)
			: Guard{bindable, state.lock()}
		{}

		inline
		Guard(Guard&& other)
			: _bindable(other._bindable)
		{ other._bindable = nullptr; }

		~Guard();
	private:
		Guard(Guard const&) = delete;
		Guard& operator =(Guard const&) = delete;
		Guard& operator =(Guard&&) = delete;
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
