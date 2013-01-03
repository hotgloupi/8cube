#ifndef  ETC_META_MATH_POWER_HPP
# define ETC_META_MATH_POWER_HPP

namespace etc { namespace meta { namespace math {

	template<typename T, T n, unsigned int p>
	struct power
	{
		static T const value = power<T, n, p - 1>::value * n;
	};

	template<typename T, T n>
	struct power<T, n, 0>
	{
		static T const value = static_cast<T>(1);
	};

}}}

#endif
