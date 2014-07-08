#ifndef  CUBE_SCENE_SCENE_HPP
# define CUBE_SCENE_SCENE_HPP

# include "fwd.hpp"

# include <cube/api.hpp>
# include <cube/gl/fwd.hpp>
# include <cube/gl/renderer/fwd.hpp>
# include <cube/resource/Resource.hpp>

# include <vector>
# include <string>

namespace cube { namespace scene {

	/**
	 * Stores meshes and materials relative to a scene.
	 */
	class CUBE_API Scene
		: public resource::Resource
	{
	public:
		struct Impl;
	private:
		std::unique_ptr<Impl> _this;

	protected:
		Scene(std::unique_ptr<Impl> self) ETC_NOEXCEPT;

	public:
		/// Build an empty scene
		Scene();

		/// Build a scene by loading a file.
		static
		ScenePtr from_file(std::string const& path);

		/// Build a scene from string (You can help the importer by providing
		/// the extension).
		static
		ScenePtr from_string(std::string const& str, std::string const& ext = "");

		Scene(Scene&& other) ETC_NOEXCEPT;

		~Scene();

	private:
		Scene(Scene const&) = delete;
		Scene& operator =(Scene const&) = delete;
		Scene& operator =(Scene&& other) = delete;

	public:
		Graph& graph() ETC_NOEXCEPT;

	public:
		physics::Engine& physics_engine() ETC_NOEXCEPT;

	public:
		typedef std::vector<gl::mesh::MeshPtr> MeshList;
		MeshList const& meshes() const ETC_NOEXCEPT;

	public:
		typedef std::vector<gl::material::MaterialPtr> MaterialList;
		MaterialList const& materials() const ETC_NOEXCEPT;

	public:
		typedef std::vector<gl::renderer::LightPtr> LightList;
		LightList const& lights() const ETC_NOEXCEPT;
	public:
		SceneViewPtr view(gl::renderer::Renderer& renderer);
		gl::renderer::DrawablePtr drawable(gl::renderer::Renderer&);
	};

}}

#endif
