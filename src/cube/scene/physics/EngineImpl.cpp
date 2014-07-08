#include "EngineImpl.hpp"

namespace cube { namespace scene { namespace physics {

	Engine::Impl::Impl()
		: broadphase{new btDbvtBroadphase()}
		, collision_configuration{new btDefaultCollisionConfiguration()}
		, collision_dispatcher{new btCollisionDispatcher(collision_configuration.get())}
		, constraint_solver{new btSequentialImpulseConstraintSolver}
		, world{
			new btDiscreteDynamicsWorld{
				collision_dispatcher.get(),
				broadphase.get(),
				constraint_solver.get(),
				collision_configuration.get(),
			}
		}
	{}

	Engine::Impl::~Impl()
	{}

}}}
