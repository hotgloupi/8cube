#ifndef CUBE_SCENE_FWD_HPP
# define CUBE_SCENE_FWD_HPP

# include "node/fwd.hpp"

# include <memory>

namespace cube { namespace scene {

	class Scene;
	typedef std::shared_ptr<Scene> ScenePtr;

	class SceneView;
	typedef std::shared_ptr<SceneView> SceneViewPtr;

	class Graph;

}}

#endif
