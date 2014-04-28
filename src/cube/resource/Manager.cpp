#include "Manager.hpp"

#include <cube/exception.hpp>

#include <etc/log.hpp>
#include <etc/path.hpp>
#include <etc/assert.hpp>

#include <boost/bimap.hpp>

#include <atomic>
#include <algorithm>
#include <limits>
#include <map>
#include <vector>

namespace cube { namespace resource {

	ETC_LOG_COMPONENT("cube.resource.Manager");

	using cube::exception::Exception;
	namespace fs = boost::filesystem;

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

		boost::bimap<boost::filesystem::path, id_type> by_path;
		std::vector<boost::filesystem::path> paths;
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
	Manager::add_path(path_type path)
	{
		ETC_TRACE.debug(*this, "Add directory", path);
		if (!path.is_absolute())
		{
			path = fs::absolute(std::move(path));
			ETC_LOG.debug(*this, "Use absolute version", path);
		}
		if (!fs::exists(path))
			throw Exception{"The path '" + path.string() + "' does not exists"};
		if (!fs::is_directory(path))
			throw Exception{"The path '" + path.string() + "' does not refer to a valid directory"};
		_this->paths.emplace_back(std::move(path));
		return *this;
	}

	Manager::path_type Manager::find(path_type path)
	{
		ETC_TRACE.debug("Searching for resource file at", path);
		if (!path.is_absolute())
		{
			for (auto& dir: _this->paths)
			{
				ETC_LOG.debug("Trying resources directory", dir);
				auto p = dir / path;
				if (fs::is_regular_file(p))
				{
					ETC_LOG.debug("Found resource at", p);
					return p;
				}
			}
			throw Exception{
				"Couldn't find any file at '" + path.string() + "'"
			};
		}
		if (fs::is_regular_file(path))
		{
			ETC_LOG.debug("Return absolute path", path);
			return path;
		}
		throw Exception{"The path '" + path.string() + "' does not refer to a resource file"};
	}

	bool Manager::loaded(path_type const& path)
	{
		return _this->by_path.left.find(this->find(path)) != _this->by_path.left.end();
	}

	Resource* Manager::_loaded_resource(path_type const& path)
	{
		auto p = this->find(path);
		auto it = _this->by_path.left.find(p);
		if (it == _this->by_path.left.end())
			return nullptr;
		return _this->resources[it->second].get();
	}

	void Manager::set_loaded(Resource& resource, path_type path)
	{
		if (resource._manager != this)
			throw Exception{"This resource is already managed by another manager"};
		if (!resource.managed())
			throw Exception{"The resource is not managed"};
		_this->by_path.insert({std::move(path), resource.id()});
	}

	void Manager::_set_loaded(id_type id, path_type p)
	{
		ETC_ASSERT_NEQ(_this->resources.find(id), _this->resources.end());
		ETC_ASSERT(p.is_absolute());
		_this->by_path.insert({std::move(p), id});
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
		_this->by_path.right.erase(resource.id());
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
		{
			_this->by_path.right.erase(id);
			_this->resources.erase(id);
		}
	}

}}
