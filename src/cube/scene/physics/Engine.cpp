#include "EngineImpl.hpp"

namespace cube { namespace scene { namespace physics {

	static gl::vector::vec3f default_gravity{0.0f, -9.81f, 0.0f};

	Engine::Engine()
		: _this{new Impl}
	{
		this->gravity(default_gravity);
	}

	Engine::~Engine()
	{}

	Engine& Engine::gravity(gl::vector::vec3f const& acceleration)
	{
		_this->world->setGravity(btVector3{acceleration.x, acceleration.y, acceleration.z});
		return *this;
	}

	gl::vector::vec3f Engine::gravity() const ETC_NOEXCEPT
	{
		auto g = _this->world->getGravity();
		return gl::vector::vec3f{g.getX(), g.getY(), g.getZ()};
	}

}}}
