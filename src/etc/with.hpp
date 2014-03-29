#ifndef  ETC_WITH_HPP
# define ETC_WITH_HPP

# include <exception>
# include <utility>

# include <etc/Expected.hpp>

namespace etc {

	template<typename Value>
	struct With;

	template<typename Fn, typename... Args>
	auto with(Fn&& fn, Args&&... args)
		-> With<decltype(fn(std::forward<Args>(args)...))>
	{
		typedef
			decltype(fn(std::forward<Args>(args)...))
			value_type;
		return With<value_type>(
			std::forward<Fn>(fn),
			std::forward<Args>(args)...
		);
	}

	template<typename Value>
	struct With
	{
		typedef Value value_type;
		typedef etc::Expected<value_type> expected_type;

		expected_type _result;

		template<typename Fn, typename... Args>
		inline
		With(Fn&& fn, Args&&... args)
			: _result(
				expected_type::of(
					std::forward<Fn>(fn),
					std::forward<Args>(args)...
				)
			)
		{}

		template<typename Fn, typename... Args>
		inline
		With& success(Fn&& fn, Args&&... args)
		{
			if (_result.valid())
				_result.reset(
					expected_type::of(
						std::forward<Fn>(fn),
						_result.value(),
						std::forward<Args>(args)...
					)
				);
			return *this;
		}

		template<typename Fn, typename... Args>
		inline
		With& failure(Fn&& fn, Args&&... args)
		{
			if (!_result.valid())
				_result.reset(
					expected_type::of(
						std::forward<Fn>(fn),
						_result.exception(),
						std::forward<Args>(args)...
					)
				);
			return *this;
		}

		template<typename Fn, typename... Args>
		inline
		With& both(Fn&& fn, Args&&... args)
		{
			_result.reset(
			 	expected_type::of(
					std::forward<Fn>(fn),
					_result,
					std::forward<Args>(args)...
				)
			);
			return *this;
		}

		Value& operator ()()
		{ return _result.value(); }

		Value const& operator ()() const
		{ return _result.value(); }

		bool valid() const ETC_NOEXCEPT { return _result.valid(); }

		expected_type const& expected() const ETC_NOEXCEPT
		{ return _result; }
	};


}

#endif
