#ifndef  ETC_EXPECTED_HPP
# define ETC_EXPECTED_HPP

# include "Expected.inl"

namespace etc {

	/**
	 * @brief Hold a value of type Value or an exception.
	 *
	 *
	 */
	template<typename Value>
	struct Expected
		: public detail::ExpectedStorage<Value>
	{
	private:
		// Storage alias.
		typedef detail::ExpectedStorage<Value> storage_type;

	private:
		// Private constructor that skip initialization.
		enum NoInit {no_init};
		explicit
		Expected(NoInit) ETC_NOEXCEPT
		{}

	public:
		/**
		 * Construct an Expected.
		 *
		 * Arguments are forwarded the the Value constructor unless:
		 *    - The only argument is an instance of an exception (and derives
		 *    from std::exception).
		 *    - The only argument is an std::exception_ptr instance.
		 *    - The only argument is an Expected instance.
		 *
		 * If the value cannot be constructed, or the argument is an exception
		 * or if the argument is an invalid Expected, the built Expected
		 * instance is invalid and the exception is stored. Otherwise, the
		 * Expected is valid and its value can be retreived safely.
		 */
		template<typename... Args>
		Expected(Args&&... args) ETC_NOEXCEPT
		{ storage_type::_init(std::forward<Args>(args)...); }

		// Those two constructors are here to allow return value optimizations.
		Expected(Expected&& other) ETC_NOEXCEPT
		{ storage_type::_init(std::move(other)); }
		Expected(Expected const& other) ETC_NOEXCEPT
		{ storage_type::_init(other); }

	private:
		Expected& operator =(Expected&& other) = delete;
		Expected& operator =(Expected const& other) = delete;

	public:
		/// Build an expected with the given arguments.
		template<typename... Args>
		static
		Expected make(Args&&... args) ETC_NOEXCEPT
		{ return Expected(std::forward<Args>(args)...); }

		/// Build an expected from the current exception.
		static
		Expected from_exception()
		{ return Expected(std::current_exception()); }

	public:
		/// Build an expected by evaluating a function. If the function
		/// returns nothing, returns a default constructed expected on success.
		template<typename Fn, typename... Args>
		static
		auto of(Fn&& fn, Args&&... args)
			-> typename std::enable_if<
				  std::is_same<
					  decltype(fn(std::forward<Args>(args)...))
					, void
				>::value == false
				, Expected
			>::type
		{
			try
			{ return Expected(fn(std::forward<Args>(args)...)); }
			catch (...)
			{ return Expected::from_exception(); }
		}

		template<typename Fn, typename... Args>
		static
		auto of(Fn&& fn, Args&&... args)
			-> typename std::enable_if<
				  std::is_same<
					  decltype(fn(std::forward<Args>(args)...))
					, void
				>::value == true
				, Expected
			>::type
		{
			try
			{ fn(std::forward<Args>(args)...); return Expected(); }
			catch (...)
			{ return Expected::from_exception(); }
		}

	public:
		/// Reset the expected. Accepts the same arguments as the constructor.
		template<typename... Args>
		void reset(Args&&... args) ETC_NOEXCEPT
		{
			storage_type::_clear();
			storage_type::_init(std::forward<Args>(args)...);
		}


	public:
		/// Throw when invalid or return the value.
		typename storage_type::lvalue_ref_type value()
		{
			this->ensure();
			return storage_type::value();
		}

		typename storage_type::lvalue_const_ref_type value() const
		{
			this->ensure();
			return storage_type::value();
		}

	public:
		/// Throw if the expected is invalid.
		void ensure() const
		{ if (!this->valid()) this->rethrow(); }

	public:
		/// Rethrow the stored exception or an instance of std::logic_error if
		/// the expected is valid.
		void rethrow() const
		{ std::rethrow_exception(this->exception()); }

	public:
		/// Check if the expected stores a kind of exception.
		template<typename Kind> bool has_exception() const
		{
			if (!this->valid())
			{
				try { this->rethrow(); }
				catch (Kind const&) { return true; }
				catch (...) {}
			}
			return false;
		}
	};

	template<typename Class, typename... Args>
	Expected<Class> make_expected(Args&&... args)
	{ return Expected<Class>(std::forward<Args>(args)...); }

	template<typename Fn, typename... Args>
	auto expected_of(Fn&& fn, Args&&... args)
		-> Expected<decltype(fn(std::forward<Args>(args)...))>
	{
		typedef decltype(fn(std::forward<Args>(args)...)) value_type;
		return Expected<value_type>::of(
			std::forward<Fn>(fn),
			std::forward<Args>(args)...
		);
	}
}

#endif
