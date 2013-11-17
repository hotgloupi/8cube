#ifndef CUBE_SCENE_FWD_HPP
# define CUBE_SCENE_FWD_HPP

# include <memory>

namespace cube { namespace scene {

	class Scene;
	typedef std::unique_ptr<Scene> ScenePtr;

}}

#endif
