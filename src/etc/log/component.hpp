#ifndef  ETC_LOG_COMPONENT_HPP
# define ETC_LOG_COMPONENT_HPP

# include <boost/preprocessor/cat.hpp>

# include <string>

# define ETC_LOG_SCOPE_COMPONENT(__name)                                      \
	struct BOOST_PP_CAT(log, __LINE__) {                                      \
		inline                                                                \
		std::string const& operator ()() const ETC_NOEXCEPT                   \
		{ static std::string n{__name}; return n; }                           \
	} const etc_log_component;                                                \
/**/

# define ETC_LOG_COMPONENT(__name)                                            \
	static inline                                                             \
	std::string const& etc_log_component() ETC_NOEXCEPT                       \
	{ static std::string n{__name}; return n; }                               \
/**/

# define ETC_LOG_SUB_COMPONENT(__name)                                        \
	static auto BOOST_PP_CAT(log, __LINE__) =                                 \
		std::string{etc_log_component()} + "#" __name;                        \
	static auto etc_log_component = [&] () -> std::string const& {            \
		return BOOST_PP_CAT(log, __LINE__);                                   \
	};                                                                        \
/**/

#endif
