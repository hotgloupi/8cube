#ifndef  CUBE_RESOURCE_MANAGER_HPP
# define CUBE_RESOURCE_MANAGER_HPP

# include "Resource.hpp"

# include <memory>
# include <unordered_map>

namespace cube { namespace resource {

	class Manager
	{
	private:
		typedef
			std::unordered_map<id_type, std::weak_ptr<Resource>>
			ResourceMap;

	private:
		ResourceMap _resources;
		id_type     _next_id;

	public:
		Manager()
			: _resources{}
			, _next_id{1}
		{}

		~Manager();

	public:
		template<typename Kind, typename... Args>
		std::shared_ptr<Kind> create(Args&&... args)
		{
			std::shared_ptr<Kind> ptr{new Kind{std::forward<Args>(args)...}};
			ptr->id(_next_id++);
			_resources.emplace(
				{ptr->id},
				{ptr}
			);
			return std::move(ptr);
		}
	};

}}

#endif

