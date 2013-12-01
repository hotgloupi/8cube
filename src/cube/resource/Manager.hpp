#ifndef  CUBE_RESOURCE_MANAGER_HPP
# define CUBE_RESOURCE_MANAGER_HPP

# include "fwd.hpp"
# include "Resource.hpp"

# include <cube/api.hpp>

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
		template<typename ResourceType>
		inline
		std::shared_ptr<ResourceType>
		manage(ResourceType& resource)
		{ return std::static_pointer_cast<ResourceType>(_manage(resource)); }

		template<typename ResourceType>
		inline
		std::shared_ptr<ResourceType>
		manage(std::shared_ptr<ResourceType>resource)
		{
			return std::static_pointer_cast<ResourceType>(
				_manage(std::static_pointer_cast<Resource>(resource))
			);
		}

	private:
		ResourcePtr _manage(Resource& resource);
		ResourcePtr _manage(ResourcePtr resource);

	public:
		/**
		 * Stop managing a resource.
		 */
		void forget(Resource& resource);

		/**
		 * Cleanup unused resources.
		 */
		void flush();
	};

}}

#endif
