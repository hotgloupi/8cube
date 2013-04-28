#include "Resource.hpp"

#include "Manager.hpp"

#include <cassert>

namespace cube { namespace resource {

	Resource::~Resource()
	{}

	void Resource::manage(Manager& manager,
	                      id_type const id)
	{
		assert(id != 0);
		if (_manager != nullptr)
			_manager->forget(*this);
		assert(_manager == nullptr);
		assert(_id == 0);

		_manager = &manager;
		_id = id;
	}

	void Resource::manage()
	{
		_manager = nullptr;
		_id = 0;
	}

}}
