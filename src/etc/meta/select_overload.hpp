#ifndef  ETC_META_SELECT_OVERLOAD_HPP
# define ETC_META_SELECT_OVERLOAD_HPP

namespace etc { namespace meta {

	template<typename... Args>
	struct select_overload
	{
		template<typename Ret, typename Class>
		static auto of(Ret (Class::*fn)(Args...)) -> decltype(fn)
		{ return fn; }

		template<typename Ret>
		static auto of(Ret (*fn)(Args...)) -> decltype(fn)
		{ return fn; }
	};

}}

#endif
