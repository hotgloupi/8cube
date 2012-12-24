#ifndef  ETC_PATH_INL
# define ETC_PATH_INL

# include "path.hpp"

namespace etc { namespace path {

	template<typename T1, typename T2, typename... Args>
	inline
	std::string absolute(T1&& arg1, T2&& arg2, Args&&... args)
	{
		return absolute(join(
			std::forward<T1>(arg1),
			std::forward<T2>(arg2),
			std::forward<Args>(args)...
		));
	}

	template<typename T1, typename T2, typename... Args>
	inline
	std::string directory_name(T1&& arg1, T2&& arg2, Args&&... args)
	{
		return directory_name(join(
			std::forward<T1>(arg1),
			std::forward<T2>(arg2),
			std::forward<Args>(args)...
		));
	}

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
