#include "Manager.hpp"

#include <cube/exception.hpp>

#include <etc/log.hpp>
#include <etc/path.hpp>

#include <atomic>
#include <algorithm>
#include <limits>
#include <map>
#include <vector>

namespace cube { namespace resource {

	ETC_LOG_COMPONENT("cube.resource.Manager");

	using cube::exception::Exception;

	namespace {

		// XXX id are garanteed to be "unique" between managers unless more
		// than max<id_type>() is reached.
		inline
		id_type next_id()
		{
			static std::atomic<id_type> current_id{0};
			assert(current_id != std::numeric_limits<id_type>::max());
			return ++current_id;
		}

	}
	struct Manager::Impl
	{
		typedef
			std::map<id_type, std::shared_ptr<Resource>>
			ResourceMap;

		std::vector<std::string> paths;
		ResourceMap resources;

		Impl()
			: resources{}
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
		if (!etc::path::exists(abs))
			throw Exception{"The path '" + path + "' does not exists"};
		if (!etc::path::is_directory(abs))
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
		auto id = resource_ptr->manage(*this, next_id());
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

	void Manager::flush()
	{
		std::vector<id_type> to_remove;
		for (auto const& pair: _this->resources)
			if (pair.second.use_count() == 1)
				to_remove.push_back(pair.first);
		ETC_LOG.debug("Flushing resources (remove",
		              to_remove.size(), "/", _this->resources.size(), ")");
		for (id_type id: to_remove)
			_this->resources.erase(id);
	}

}}
