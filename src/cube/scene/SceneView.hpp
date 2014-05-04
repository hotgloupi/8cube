#ifndef  CUBE_SCENE_SCENEVIEW_HPP
# define CUBE_SCENE_SCENEVIEW_HPP

# include "fwd.hpp"

# include <cube/gl/renderer/Drawable.hpp>

namespace cube { namespace scene {

	class SceneView
		: public gl::renderer::Drawable
	{
	public:
		struct Impl;
	private:
		std::unique_ptr<Impl> _this;
	public:
		SceneView(ScenePtr scene);
		~SceneView();

	public:
		void _draw(gl::renderer::Painter& painter) override;
	};

}}

#endif

