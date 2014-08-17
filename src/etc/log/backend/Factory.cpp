#include "Factory.hpp"
#include "Console.hpp"

#include <cstdlib>
#include <mutex>

namespace etc { namespace log { namespace backend {

	struct Factory::Impl
	{
		std::shared_ptr<Interface> default_backend;
		std::mutex lock;
	};

	Factory::Factory()
		: _this{new Impl}
	{}

	Factory::~Factory()
	{}

	std::shared_ptr<Interface> Factory::default_backend()
	{
		std::lock_guard<std::mutex> guard{_this->lock};
		if (_this->default_backend == nullptr)
			_this->default_backend.reset(new Console);
		return _this->default_backend;
	}

	static Factory* instance = nullptr;

	static void cleanup()
	{
		if (instance != nullptr)
			delete instance;
		instance = nullptr;
	}

	Factory& factory()
	{
		static bool cleanup_registered = false;
		if (instance == nullptr) // XXX not threadsafe, should use CAS
		{
			instance = new Factory();
			if (!cleanup_registered)
			{
				cleanup_registered = true;
				std::atexit(&cleanup);
			}
		}
		return *instance;
	}

}}}
