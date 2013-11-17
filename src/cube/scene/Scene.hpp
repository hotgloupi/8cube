#ifndef  CUBE_SCENE_SCENE_HPP
# define CUBE_SCENE_SCENE_HPP

# include "fwd.hpp"

# include <string>

namespace cube { namespace scene {

	class Scene
	{
	private:
		struct Impl;
		std::unique_ptr<Impl> _this;

	public:
		/// Build a scene by loading a file.
		Scene(std::string const& path);

		~Scene();
	};

}}

#endif
