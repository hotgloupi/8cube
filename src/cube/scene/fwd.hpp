#ifndef CUBE_SCENE_FWD_HPP
# define CUBE_SCENE_FWD_HPP

# include <memory>

namespace cube { namespace scene {

	class Scene;
	typedef std::shared_ptr<Scene> ScenePtr;

	class SceneView;
	typedef std::shared_ptr<SceneView> SceneViewPtr;

	class Graph;

	class Node;

	template <typename NodeType>
	class NodeVisitor;

}}

#endif
