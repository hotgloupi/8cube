#ifndef  ETC_STACK_PTR_HPP
# define ETC_STACK_PTR_HPP

# include "compiler.hpp"

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
		char _storage[sizeof(T)];

	public:
		inline explicit
		stack_ptr(stack_ptr_no_init_t) ETC_NOEXCEPT
			: _valid{false}
		{}

		template<typename... Args>
		inline explicit
		stack_ptr(Args&&... args)
			ETC_NOEXCEPT_IF(std::is_nothrow_constructible<T, Args...>::value)
			: _valid{true}
		{ new (_storage) T{std::forward<Args>(args)...}; }

		inline
		stack_ptr(stack_ptr&& other)
			ETC_NOEXCEPT_IF(std::is_nothrow_move_constructible<T>::value)
			: _valid{other._valid}
		{
			if (_valid)
				// We just move the value of the other stack_ptr, whose dtor
				// will call the _value dtor.
				new (_storage) T(std::move(other.get()));
		}

		inline
		stack_ptr(stack_ptr const& other)
			ETC_NOEXCEPT_IF(std::is_nothrow_copy_constructible<T>::value)
			: _valid{other._valid}
		{
			if (_valid)
				new (_storage) T(other.get());
		}

		inline
		~stack_ptr()
		{ this->clear(); }

		inline
		void clear() ETC_NOEXCEPT
		{ if (_valid) { this->get().~T(); _valid = false; }}

		template<typename... Args>
		void reset(Args&&... args)
			ETC_NOEXCEPT_IF(std::is_nothrow_constructible<T, Args...>::value)
		{
			this->clear();
			new (&_storage) T(std::forward<Args>(args)...);
			_valid = true;
		}

		inline operator bool() const ETC_NOEXCEPT { return _valid; }

		inline value_type& get() ETC_NOEXCEPT
		{ return *reinterpret_cast<value_type*>(_storage);}
		inline value_type const& get() const ETC_NOEXCEPT
		{ return *reinterpret_cast<value_type const*>(_storage);}

		inline
		value_type* operator ->() ETC_NOEXCEPT { return &this->get(); }
		inline
		value_type const* operator ->() const ETC_NOEXCEPT { return &this->get(); }

		inline
		value_type& operator *() ETC_NOEXCEPT { return this->get(); }
		inline
		value_type const& operator *() const ETC_NOEXCEPT { return this->get(); }
	};

}

#endif
