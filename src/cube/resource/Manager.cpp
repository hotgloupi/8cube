#include "Manager.hpp"

#include <cube/exception.hpp>

#include <etc/path.hpp>

#include <unordered_map>
#include <vector>

namespace cube { namespace resource {

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
	{}

	Manager&
	Manager::add_path(std::string const& path)
	{
		std::string abs =  etc::path::absolute(path);
		if (not etc::path::exists(abs))
			throw Exception{"The path '" + path + "' does not exists"};
		if (not etc::path::is_directory(abs))
			throw Exception{"The path '" + path + "' does not refer to a valid directory"};
		_this->paths.push_back(abs);
	}

	void
	Manager::manage(Resource& resource)
	{
		resource.manage(*this, ++_this->next_id);
		_this->resources.emplace(
			resource.id(),
			resource.shared_from_this()
		);
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
