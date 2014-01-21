#ifndef CUBE_SCENE_FWD_HPP
# define CUBE_SCENE_FWD_HPP

# include <memory>

namespace cube { namespace scene {

	class Scene;
	typedef std::shared_ptr<Scene> ScenePtr;

	class Graph;

	class Node;
	typedef std::unique_ptr<Node> NodePtr;

	class GroupNode;


}}

#endif
