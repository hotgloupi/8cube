#include "Manager.hpp"

#include <cube/exception.hpp>

#include <etc/log.hpp>
#include <etc/path.hpp>

#include <unordered_map>
#include <vector>

namespace cube { namespace resource {

	ETC_LOG_COMPONENT("cube.resource.Manager");

	using cube::exception::Exception;

	struct Manager::Impl
	{
		typedef
			std::unordered_map<id_type, std::shared_ptr<Resource>>
			ResourceMap;

		std::vector<std::string> paths;
		ResourceMap resources;
		id_type     next_id;

		Impl()
			: resources{}
			, next_id{1}
		{}
	};

	Manager::Manager()
		: _this{new Impl}
	{}

	Manager::~Manager()
	{
		for (auto& pair: _this->resources)
		{
			pair.second->manage();
			if (pair.second.use_count() > 1)
				ETC_LOG.warn("Destroying", *this, "earlier than", *pair.second);
		}
	}

	Manager&
	Manager::add_path(std::string const& path)
	{
		std::string abs =  etc::path::absolute(path);
		if (not etc::path::exists(abs))
			throw Exception{"The path '" + path + "' does not exists"};
		if (not etc::path::is_directory(abs))
			throw Exception{"The path '" + path + "' does not refer to a valid directory"};
		_this->paths.push_back(abs);
		return *this;
	}

	ResourcePtr
	Manager::_manage(Resource& resource)
	{
		auto resource_ptr = resource.shared_from_this();
		if (resource_ptr.use_count() == 1)
			throw Exception{"This resource has been created on the stack"};
		return this->manage(std::move(resource_ptr));
	}

	ResourcePtr
	Manager::_manage(ResourcePtr resource_ptr)
	{
		auto id = resource_ptr->manage(*this, ++_this->next_id);
		auto const& it = _this->resources.emplace(id, std::move(resource_ptr));
		if (it.second == false)
			throw Exception{"Couldn't store the resource"};
		return it.first->second;
	}

	void
	Manager::forget(Resource& resource)
	{
		if (!resource.managed())
			throw Exception{"The resource is not managed"};
		auto it = _this->resources.find(resource.id());
		if (it == _this->resources.end())
			throw Exception{"The resource is not managed by this manager"};
		_this->resources.erase(it);
		resource.manage();
	}

}}
