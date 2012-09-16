#ifndef  CUBE_GL_RENDERER_BINDABLE_HPP
# define CUBE_GL_RENDERER_BINDABLE_HPP

# include "fwd.hpp"

# include <etc/types.hpp>

namespace cube { namespace gl { namespace renderer {

	struct BindableBase
	{
	protected:
		etc::size_type __bound;

	public:
		BindableBase()
			: __bound{0}
		{}

		virtual
		~BindableBase()
		{}

	public:
		/**
		 * @brief abstract update method.
		 *
		 * This method is called whenever a matrix is updated in the current
		 * state.
		 */
		virtual
		void update(MatrixKind, matrix_type const&)
		{}

	protected:
		/**
		 * @brief   Unbind the bindable.
		 *
		 * This method is called by the painter only if the bindable actually
		 * *needs* to be unbound. That means implementations of this method do
		 * not need to maintain or check a bound state.
		 *
		 * WARNING: This method should only be used in the subclasses
		 * constructors. If you need to bind/unbind in a subclass method, use
		 * the Bindable<>::Guard class.
		 *
		 */
		virtual
		void _unbind() = 0;

	protected:
		void _set_bound(bool value);

	private:
		inline
		void __unbind()
		{
			_set_bound(false);
			_unbind();
		}

	protected:
		struct GuardBase
		{
		protected:
			BindableBase* _bindable;

		protected:
			GuardBase(BindableBase* bindable)
				: _bindable(bindable)
			{}

		public:
			~GuardBase() // WARNING : dtor is not virtual
			{ if (_bindable != nullptr) _bindable->__unbind(); }
		};
		friend struct GuardBase;
		friend class Painter;
	};

	template<typename... Args>
	struct Bindable
		: public BindableBase
	{
	protected:
		virtual
		void _bind(Args&...) = 0;


		inline
		bool _bound() const
		{ return __bound > 0; }


	protected:
		/**
		 * @brief   Lazy bind guard.
		 *
		 * Use RIIA pattern to bind/unbind a Bindable if not already bound. In
		 * other words, it will unbind the Bindable instance only if it were
		 * bound in the first place.
		 */
		struct Guard
			: public GuardBase
		{
		public:
			explicit
			Guard(Bindable& bindable, Args&... args)
				: GuardBase(!bindable._bound() ? &bindable : nullptr)
			{ if (_bindable != nullptr) bindable.__bind(args...); }
		};
	private:
		// __bind method is called by Painter and Guard
		inline
		void __bind(Args&... args)
		{
			_set_bound(true);
			_bind(args...);
		}
		friend struct Guard;
		friend class Painter;
	};

}}}

#endif

