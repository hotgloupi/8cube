#ifndef  ETC_PATH_HPP
# define ETC_PATH_HPP

# include <string>
# include <vector>

namespace etc { namespace path {

	std::string absolute(std::string const& path);

	template<typename T1, typename T2, typename... Args>
	inline
	std::string absolute(T1&& arg1, T2&& arg2, Args&&... args);

	std::string directory_name(std::string const& path);

	template<typename T1, typename T2, typename... Args>
	inline
	std::string directory_name(T1&& arg1, T2&& arg2, Args&&... args);

	std::string join(std::vector<std::string> const& parts);

	template<typename T1, typename T2, typename... Args>
	inline
	std::string join(T1&& arg1, T2&& arg2, Args&&... args);

}}

# include "path.inl"

#endif
