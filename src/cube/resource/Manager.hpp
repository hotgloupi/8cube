#ifndef  CUBE_RESOURCE_MANAGER_HPP
# define CUBE_RESOURCE_MANAGER_HPP

# include "fwd.hpp"
# include "Resource.hpp"

# include <cube/api.hpp>

# include <wrappers/boost/filesystem.hpp>

# include <string>

namespace cube { namespace resource {

	class CUBE_API Manager
	{
	public:
		typedef boost::filesystem::path path_type;
	private:
		struct Impl;
		std::unique_ptr<Impl> _this;

	public:
		Manager();
		~Manager();

		Manager& add_path(path_type path);

	public:
		template<typename Kind, typename... Args>
		std::shared_ptr<Kind>
		create(Args&&... args)
		{
			std::shared_ptr<Kind> ptr{new Kind{std::forward<Args>(args)...}};
			this->manage(*ptr);
			return std::move(ptr);
		}

		template<typename Kind>
		std::shared_ptr<Kind>
		load(path_type const& path)
		{
			auto p = this->find(path);
			if (Resource* ptr = _loaded_resource(path))
				return ptr->ptr();
			std::shared_ptr<Kind> ptr = this->create<Kind>(p);
			_set_loaded(ptr->id(), p);
			return ptr;
		}

		bool loaded(path_type const& path);

		path_type find(path_type const& path);
		ResourcePtr& get(path_type const& path);

		void set_loaded(Resource& resource, path_type path);
	private:
		Resource* _loaded_resource(path_type const& p);
		void _set_loaded(id_type id, path_type p);

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
