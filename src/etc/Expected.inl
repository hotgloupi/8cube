#ifndef  ETC_EXPECTED_INL
# define ETC_EXPECTED_INL

# include <etc/assert.hpp>
# include <etc/compiler.hpp>

# include <boost/config.hpp>

# include <exception>
# include <stdexcept>
# include <type_traits>
# include <utility>

namespace etc { namespace detail {

	template<typename Value>
	struct ExpectedStorage
	{
	public:
		typedef Value value_type;
		static_assert(
			!std::is_convertible<value_type, std::exception>::value,
			"Cannot expect an exception"
		);
		typedef value_type& lvalue_ref_type;
		typedef value_type const& lvalue_const_ref_type;

	protected:
		union
		{
			char _exception[sizeof(std::exception_ptr)];
			char _value[sizeof(value_type)];
		};
		bool _valid;

		template<typename... Args>
		void _init_value(Args&&... args) ETC_NOEXCEPT
		{
			try
			{
				new (_value) Value(std::forward<Args>(args)...);
				_valid = true;
			}
			catch (...)
			{
				new (_exception) std::exception_ptr(std::current_exception());
				_valid = false;
			}
		}

#ifndef BOOST_MSVC
		template<typename... Args>
		 auto _init(Args&&... args) ETC_NOEXCEPT
		 	-> typename std::enable_if<(sizeof...(Args) != 1)>::type
		{ _init_value(std::forward<Args>(args)...); }
#else
		void _init()
		{ _init_value(); }
		template<typename T1, typename T2, typename... Args>
		void _init(T1&& arg1, T2&& arg2, Args&&... args) ETC_NOEXCEPT
		{
			_init_value(
				std::forward<T1>(arg1),
				std::forward<T2>(arg2),
				std::forward<Args>(args)...
			);
		}
#endif

		void _init(value_type&& value) ETC_NOEXCEPT
		{ _init_value(std::move(value)); }

		void _init(value_type const& value) ETC_NOEXCEPT
		{ _init_value(value); }

		template<typename T>
		typename std::enable_if<
			std::is_base_of<ExpectedStorage, T>::value
		>::type
		_init(T&& other) ETC_NOEXCEPT
		{
			if (other.valid())
				_init(std::move(other.value()));
			else
				_init(other.exception());
		}

		template<typename T>
		typename std::enable_if<
			std::is_base_of<ExpectedStorage, T>::value
		>::type
		_init(T const& other) ETC_NOEXCEPT
		{
			if (other.valid())
				_init(other.value());
			else
				_init(other.exception());
		}

		template<typename E>
		typename std::enable_if<
			  std::is_base_of<std::exception, E>::value
		>::type
		_init(E const& err) ETC_NOEXCEPT
		{ _init(std::make_exception_ptr(err)); }

		void _init(std::exception_ptr error) ETC_NOEXCEPT
		{
			new (_exception) std::exception_ptr(std::move(error));
			_valid = false;
		}

		void _clear() ETC_NOEXCEPT
		{
			if (_valid)
				_as_value().~Value();
			else
				_as_exception().~exception_ptr();
			_valid = false;
		}

	public:
		~ExpectedStorage() { _clear(); }

	public:
		bool valid() const ETC_NOEXCEPT { return _valid; }
	public:
		value_type& value() { return _as_value(); }

		value_type const& value() const
		{ return _as_value(); }

	public:
		std::exception_ptr exception() const
		{
			if (_valid)
				throw std::logic_error("No exception available");
			ETC_ASSERT_NEQ(_as_exception(), nullptr);
			return _as_exception();
		}

	private:
		std::exception_ptr& _as_exception()
		{ return *reinterpret_cast<std::exception_ptr*>(_exception); }
		std::exception_ptr const& _as_exception() const
		{ return *reinterpret_cast<std::exception_ptr const*>(_exception); }

	private:
		value_type& _as_value()
		{ return *reinterpret_cast<value_type*>(_value); }
		value_type const& _as_value() const
		{ return *reinterpret_cast<value_type const*>(_value); }
	};

	template<>
	struct ExpectedStorage<void>
	{
	public:
		typedef void lvalue_ref_type;
		typedef void lvalue_const_ref_type;
	private:
		std::exception_ptr _exception;

	public:
		bool valid() const ETC_NOEXCEPT { return !_exception; }
		std::exception_ptr exception() const { return _exception; }

	protected:
		void _clear() ETC_NOEXCEPT
		{ _exception = std::exception_ptr(); }

		void _init() ETC_NOEXCEPT
		{ _clear(); }

		template<typename T>
		typename std::enable_if<
			std::is_base_of<ExpectedStorage, T>::value
		>::type
		_init(T&& other) ETC_NOEXCEPT
		{
			if (other.valid())
				_init();
			else
				_init(other.exception());
		}

		template<typename T>
		typename std::enable_if<
			std::is_base_of<ExpectedStorage, T>::value
		>::type
		_init(T const& other) ETC_NOEXCEPT
		{
			if (other.valid())
				_init();
			else
				_init(other.exception());
		}

		template<typename E>
		typename std::enable_if<
			  std::is_base_of<std::exception, E>::value
		>::type
		_init(E const& err) ETC_NOEXCEPT
		{ _init(std::make_exception_ptr(err)); }

		void _init(std::exception_ptr error) ETC_NOEXCEPT
		{ _exception = std::move(error); }
	};

}} // !etc::detail

#endif
