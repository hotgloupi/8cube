#ifndef  ETC_PATH_INL
# define ETC_PATH_INL

# include "path.hpp"

namespace etc { namespace path {

# define ETC_PATH_JOINED(__ret, __name)                                       \
	template<typename... Args>                                                \
	inline                                                                    \
	__ret __name(std::string arg1, std::string arg2, Args&&... args)          \
	{                                                                         \
		return __name(join(                                                   \
			std::move(arg1),                                  \
			std::move(arg2),                                  \
			std::forward<Args>(args)...                                       \
		));                                                                   \
	}                                                                         \
/**/

	ETC_PATH_JOINED(std::string, absolute);
	ETC_PATH_JOINED(bool, exists);
	ETC_PATH_JOINED(std::string, directory_name);
	ETC_PATH_JOINED(bool, is_directory);

# undef ETC_PATH_JOINED

	inline std::string join(std::string s) { return std::move(s); }

	template<typename... Args>
	inline
	std::string join(std::string arg1, std::string arg2, Args&&... args)
	{
# ifdef BOOST_MSVC // Internal compiler error
		std::vector<std::string> to_join;
		to_join.emplace_back(std::move(arg1));
		to_join.emplace_back(std::move(arg2));
		return join(join(to_join), std::forward<Args>(args)...);
# else
		return join(std::vector<std::string>{
			std::move(arg1),
			std::move(arg2),
			std::forward<Args>(args)...
		});
# endif
	}

}}

#endif
