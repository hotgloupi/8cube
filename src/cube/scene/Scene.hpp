#ifndef  CUBE_SCENE_SCENE_HPP
# define CUBE_SCENE_SCENE_HPP

# include "fwd.hpp"

# include <cube/api.hpp>
# include <cube/gl/fwd.hpp>

# include <vector>
# include <string>

namespace cube { namespace scene {

	class CUBE_API Scene
	{
	private:
		struct Impl;
		std::unique_ptr<Impl> _this;

	protected:
		Scene(std::unique_ptr<Impl> self) noexcept;

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

		Scene(Scene&& other) noexcept;

		~Scene();

	private:
		Scene(Scene const&) = delete;
		Scene& operator =(Scene const&) = delete;
		Scene& operator =(Scene&& other) = delete;

	public:
		Graph& graph() noexcept;

	public:
		typedef std::vector<gl::mesh::MeshPtr> MeshList;
		MeshList const& meshes() const noexcept;

	public:
		typedef std::vector<gl::material::MaterialPtr> MaterialList;
		MaterialList const& materials() const noexcept;
	};

}}

#endif
