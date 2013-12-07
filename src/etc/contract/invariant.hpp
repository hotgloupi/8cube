#ifndef  ETC_CONTRACT_INVARIANT_HPP
# define ETC_CONTRACT_INVARIANT_HPP

# define ETC_CONTRACT_SCOPE_INVARIANT \
	auto BOOST_PP_CAT(check_invariant_, __LINE__) =                           \
	::etc::contract::invariant::scope_helper{} + [&]                          \


# define ETC_CONTRACT_CLASS_INVARIANT() \
	ETC_CONTRACT_SCOPE_INVARIANT{ this->_invariant(); }

# include <etc/compiler.hpp>

# include <boost/preprocessor/cat.hpp>

namespace etc { namespace contract { namespace invariant {

	template<typename Check>
	class Scope
	{
	private:
		Check _invariant;

	public:
		template<typename F>
		explicit Scope(F&& f)
			: _invariant{std::forward<F>(f)}
		{ _invariant(); }

		~Scope() ETC_NOEXCEPT_IF(false)
		{ _invariant(); }
	};

	struct scope_helper
	{
		template<typename Lambda>
		inline
		Scope<Lambda> operator +(Lambda&& lambda)
		{ return Scope<Lambda>{std::forward<Lambda>(lambda)}; }
	};

}}}

#endif
