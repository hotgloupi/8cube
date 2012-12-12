#ifndef  ETC_MEMORY_HPP
# define ETC_MEMORY_HPP

# include <memory>

namespace etc {

	/**
	 * @brief Create a unique_ptr with given arguments.
	 */
	template<typename T, typename... Args>
	std::unique_ptr<T> make_unique(Args&&... args)
	{
		return std::unique_ptr<T>{new T{std::forward<Args>(args)...}};
	}

}

#endif
