#ifndef  CUBE_SCENE_HPP
# define CUBE_SCENE_HPP

# include "scene/Scene.hpp"

namespace cube { namespace scene {

	inline
	ScenePtr load(std::string const& path)
	{ return ScenePtr{new Scene{path}}; }

}}

#endif
