#ifndef  ETC_TYPES_HPP
# define ETC_TYPES_HPP

# include "api.hpp"
# include "compiler.hpp"

# include <cstddef>
# include <cstdint>
# include <string>
# include <typeinfo>


# define ETC_TYPE_STRING(__T)                                                 \
	::etc::demangle<etc::no_throw_policy>(typeid(__T).name())                 \
/**/

namespace etc {

	typedef uint_fast32_t   size_type;
	typedef size_t          memsize_type;

	enum throw_policy
	{
		no_throw_policy,
		can_throw_policy,
	};

	template<throw_policy tp = can_throw_policy>
	ETC_API
	std::string demangle(std::string const& sym)
		ETC_NOEXCEPT_IF(tp == no_throw_policy);

	bool demangle(std::string const& sym,
	              std::string& res,
	              std::string& error) ETC_NOEXCEPT;

}

#endif

