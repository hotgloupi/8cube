#ifndef  ETC_PATH_HPP
# define ETC_PATH_HPP

# include "api.hpp"

# include <string>
# include <vector>

namespace etc { namespace path {

// Dump both version of function: one that accepts a path, and one that accepts
// parts of a path.
# define ETC_PATH_CALL(__ret, __name)                                         \
	ETC_API __ret __name(std::string const& path);                            \
	                                                                          \
	template<typename T1, typename T2, typename... Args>                      \
	inline                                                                    \
	__ret __name(T1&& arg1, T2&& arg2, Args&&... args);                       \
/**/

	/**
	 * The absolute path.
	 */
	ETC_PATH_CALL(std::string, absolute);

	/**
	 * Returns true if the path refers to a filesystem entity.
	 */
	ETC_PATH_CALL(bool, exists);

	/**
	 * Returns the path of the deepest directory.
	 */
	ETC_PATH_CALL(std::string, directory_name);

	/**
	 * Returns true if the path refers to a valid directory.
	 */
	ETC_PATH_CALL(bool, is_directory);

	/**
	 * Join parts of a path.
	 */
	ETC_API std::string join(std::vector<std::string> const& parts);

	template<typename T1, typename T2, typename... Args>
	inline
	std::string join(T1&& arg1, T2&& arg2, Args&&... args);

	/**
	 * @brief Split a path into one or more parts.
	 *
	 * Trailing separators are ignored. Redundant separators are merged. If the
	 * path is absolute, the first token is an empty string on unices or a
	 * drive on windows.
	 */
	ETC_API std::vector<std::string> split(std::string path);

	/**
	 * Test whether a filename or pathname matches a shell-style pattern.
	 */
	ETC_API bool match(std::string const& pattern,
	                   std::string const& str);

}}

# include "path.inl"

#endif
