#ifndef  ETC_TYPES_HPP
# define ETC_TYPES_HPP

# include <cstddef>
# include <cstdint>
# include <string>
# include <typeinfo>


# define ETC_TYPE_STRING(__T)                                                 \
	::etc::demangle(typeid(__T).name())                                       \
/**/

namespace etc {

	std::string demangle(std::string const& sym);
	bool
	demangle(std::string const& sym,
	         std::string& res,
	         std::string& error) throw();

	typedef uint_fast32_t   size_type;
	typedef size_t          memsize_type;
}

#endif

