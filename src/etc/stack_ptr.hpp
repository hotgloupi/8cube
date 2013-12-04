#ifndef  ETC_STACK_PTR_HPP
# define ETC_STACK_PTR_HPP

# include <utility>
# include <type_traits>

namespace etc {

	enum stack_ptr_no_init_t {stack_ptr_no_init};

	template<typename T>
	struct stack_ptr
	{
	public:
		typedef T value_type;
	private:
		bool _valid;
		union { T _value; };

	public:
		inline explicit
		stack_ptr(stack_ptr_no_init_t) noexcept
			: _valid{false}
		{}

		template<typename... Args>
		inline explicit
		stack_ptr(Args&&... args)
			noexcept(std::is_nothrow_constructible<T, Args...>::value)
			: _valid{true}
			, _value{std::forward<Args>(args)...}
		{}

		inline
		stack_ptr(stack_ptr&& other)
			noexcept(std::is_nothrow_move_constructible<T>::value)
			: _valid{other._valid}
		{
			if (_valid)
				// We just move the value of the other stack_ptr, whose dtor
				// will call the _value dtor.
				new (&_value) T(std::move(other._value));
		}

		inline
		stack_ptr(stack_ptr const& other)
			noexcept(std::is_nothrow_copy_constructible<T>::value)
			: _valid{other._valid}
		{
			if (_valid)
				new (&_value) T(other._value);
		}

		inline
		~stack_ptr()
		{ this->clear(); }

		inline
		void clear() noexcept
		{ if (_valid) { _value.~T(); _valid = false; }}

		template<typename... Args>
		void reset(Args&&... args)
			noexcept(std::is_nothrow_constructible<T, Args...>::value)
		{
			this->clear();
			new (&_value) T(std::forward<Args>(args)...);
			_valid = true;
		}

		inline operator bool() const noexcept { return _valid; }

		inline
		value_type* operator ->() noexcept { return &_value; }

		inline
		value_type const* operator ->() const noexcept { return &_value; }

		inline
		value_type& operator *() noexcept { return _value; }

		inline
		value_type const& operator *() const noexcept { return _value; }
	};

}

#endif
