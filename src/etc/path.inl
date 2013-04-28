#ifndef  ETC_PATH_INL
# define ETC_PATH_INL

# include "path.hpp"

namespace etc { namespace path {

# define ETC_PATH_JOINED(__ret, __name)                                       \
	template<typename T1, typename T2, typename... Args>                      \
	inline                                                                    \
	__ret __name(T1&& arg1, T2&& arg2, Args&&... args)                        \
	{                                                                         \
		return __name(join(                                                   \
			std::forward<T1>(arg1),                                           \
			std::forward<T2>(arg2),                                           \
			std::forward<Args>(args)...                                       \
		));                                                                   \
	}                                                                         \
/**/

	ETC_PATH_JOINED(std::string, absolute);
	ETC_PATH_JOINED(bool, exists);
	ETC_PATH_JOINED(std::string, directory_name);
	ETC_PATH_JOINED(bool, is_directory);

# undef ETC_PATH_JOINED

	template<typename T1, typename T2, typename... Args>
	inline
	std::string join(T1&& arg1, T2&& arg2, Args&&... args)
	{
		return join({
			std::forward<T1>(arg1),
			std::forward<T2>(arg2),
			std::forward<Args>(args)...
		});
	}

}}

#endif
