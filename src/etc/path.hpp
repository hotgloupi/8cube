#ifndef  ETC_PATH_HPP
# define ETC_PATH_HPP

# include "api.hpp"

# include <string>
# include <vector>

namespace etc { namespace path {

	ETC_API std::string absolute(std::string const& path);

	template<typename T1, typename T2, typename... Args>
	inline
	std::string absolute(T1&& arg1, T2&& arg2, Args&&... args);

	ETC_API std::string directory_name(std::string const& path);

	template<typename T1, typename T2, typename... Args>
	inline
	std::string directory_name(T1&& arg1, T2&& arg2, Args&&... args);

	ETC_API std::string join(std::vector<std::string> const& parts);

	template<typename T1, typename T2, typename... Args>
	inline
	std::string join(T1&& arg1, T2&& arg2, Args&&... args);

}}

# include "path.inl"

#endif
