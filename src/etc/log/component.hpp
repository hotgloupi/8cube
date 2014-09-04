#ifndef  ETC_LOG_COMPONENT_HPP
# define ETC_LOG_COMPONENT_HPP

# include "fwd.hpp"
# include "Level.hpp"

# include <boost/preprocessor/cat.hpp>

# include <string>

# define ETC_LOG_SCOPE_COMPONENT(__name)                                      \
	struct BOOST_PP_CAT(log, __LINE__) {                                      \
		BOOST_PP_CAT(log, __LINE__)() {}                                      \
		inline                                                                \
		etc::log::Component const& operator ()() const ETC_NOEXCEPT           \
		{ static etc::log::Component const& c = etc::log::component(__name); return c; }\
	} const etc_log_component;                                                \
/**/

# define ETC_LOG_COMPONENT(__name)                                            \
	static inline                                                             \
	etc::log::Component const& etc_log_component() ETC_NOEXCEPT               \
	{ static etc::log::Component const& c = etc::log::component(__name); return c; }\
/**/

namespace etc { namespace log {

	struct Component
	{
		std::string const name;
		Level             level;
		bool              enabled;
		Logger&           logger;
	};

	ETC_API Component& component(std::string const& name);

}}

#endif
