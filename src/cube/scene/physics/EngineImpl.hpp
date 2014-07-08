#ifndef  CUBE_SCENE_PHYSICS_ENGINEIMPL_HPP
# define CUBE_SCENE_PHYSICS_ENGINEIMPL_HPP

# include "Engine.hpp"

#include <btBulletDynamicsCommon.h>

namespace cube { namespace scene { namespace physics {

	struct Engine::Impl
	{
		std::unique_ptr<btBroadphaseInterface> broadphase;
		std::unique_ptr<btDefaultCollisionConfiguration> collision_configuration;
		std::unique_ptr<btCollisionDispatcher> collision_dispatcher;
		std::unique_ptr<btSequentialImpulseConstraintSolver> constraint_solver;
		std::unique_ptr<btDiscreteDynamicsWorld> world;

		Impl();
		~Impl();
	};

}}}

#endif
