#ifndef  CUBE_RESOURCE_FWD_HPP
# define CUBE_RESOURCE_FWD_HPP

# include <memory>

namespace cube { namespace resource {

	class Manager;
	class Resource;
	typedef std::shared_ptr<Resource> ResourcePtr;

}}

#endif
