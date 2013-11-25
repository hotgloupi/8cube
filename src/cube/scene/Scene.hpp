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

	protected:
		Scene(std::unique_ptr<Impl> self) noexcept;

	public:
		/// Build an empty scene
		Scene();

		/// Build a scene by loading a file.
		static
		Scene from_file(std::string const& path);

		/// Build a scene from string (You can help the importer by providing
		/// the extension).
		static
		Scene from_string(std::string const& str, std::string const& ext = "");

		Scene(Scene&& other) noexcept;

		~Scene();

	public:
		Graph& graph() noexcept;

	private:
		Scene(Scene const&) = delete;
		Scene& operator =(Scene const&) = delete;
		Scene& operator =(Scene&& other) = delete;
	};

}}

#endif
