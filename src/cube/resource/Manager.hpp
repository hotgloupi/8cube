#ifndef  CUBE_RESOURCE_MANAGER_HPP
# define CUBE_RESOURCE_MANAGER_HPP

# include "Resource.hpp"

# include <cube/api.hpp>

# include <memory>
# include <string>

namespace cube { namespace resource {

	class CUBE_API Manager
	{
	private:
		struct Impl;
		std::unique_ptr<Impl> _this;

	public:
		Manager();
		~Manager();

		Manager& add_path(std::string const& path);

	public:
		template<typename Kind, typename... Args>
		std::shared_ptr<Kind>
		create(Args&&... args)
		{
			std::shared_ptr<Kind> ptr{new Kind{std::forward<Args>(args)...}};
			this->manage(*ptr);
			return std::move(ptr);
		}

	public:
		/**
		 * Give a resource to a manager.
		 */
		void manage(Resource& resource);

		/**
		 * Stop managing a resource.
		 */
		void forget(Resource& resource);
	};

}}

#endif
