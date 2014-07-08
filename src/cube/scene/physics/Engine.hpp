#ifndef  CUBE_SCENE_PHYSICS_ENGINE_HPP
# define CUBE_SCENE_PHYSICS_ENGINE_HPP

# include <cube/gl/vector.hpp>

# include <memory>

namespace cube { namespace scene { namespace physics {

	class Engine
	{
	private:
		struct Impl;

	private:
		std::unique_ptr<Impl> _this;

	public:
		Engine();
		~Engine();

	public:
		Engine& gravity(gl::vector::vec3f const& acceleration);
		gl::vector::vec3f gravity() const ETC_NOEXCEPT;
	};

}}}

#endif
