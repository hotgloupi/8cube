#ifndef  CUBE_GL_FONT_HPP
# define CUBE_GL_FONT_HPP

# include "renderer/fwd.hpp"
# include "renderer/Bindable.hpp"

# include <etc/types.hpp>

# include <memory>
# include <string>

namespace cube { namespace gl { namespace font {

# define CUBE_GL_FONT_DEFAULT_NAME "Ubuntu Mono"
# define CUBE_GL_FONT_DEFAULT_SIZE 12

	class Font
	{
	private:
		struct Impl;
		std::unique_ptr<Impl> _impl;

	public:
		explicit
		Font(renderer::Renderer& renderer,
		     std::string const& name = CUBE_GL_FONT_DEFAULT_NAME,
		     etc::size_type size = CUBE_GL_FONT_DEFAULT_SIZE);
		~Font();

		/**
		 * Generate a vertex buffer corresponding to a string.
		 *
		 * The vertex buffer returned is filled with vertices and font texture
		 * coordinates, and finalized.
		 */
		template<typename CharType>
		std::unique_ptr<renderer::VertexBuffer>
		generate_text(std::basic_string<CharType> const& str);

		void bind(renderer::Painter& painter,
		          renderer::ShaderProgramParameter& sampler);
		void unbind(renderer::Painter& painter);
	};

	/**
	 * @brief Returns wether or not the file should be loadable.
	 */
	bool can_load_file(std::string const& path);

	struct FontInfos;
	/**
	 * @brief Retreive infos for a speficied font file.
	 *
	 * This function does return a valid FontInfos structure
	 * or throws on any error.
	 */
	std::unique_ptr<FontInfos>
	get_infos(std::string const& path);

	struct FontInfos
	{
	public:
		std::string const path;
		std::string const family_name;
		std::string const style_name;

		~FontInfos();

	private:
		FontInfos(std::string const& path,
		          std::string const& family_name,
		          std::string const& style_name);
		struct Impl;
		std::unique_ptr<Impl> _impl;

		friend
		std::unique_ptr<FontInfos>
		get_infos(std::string const& path);
	};

}}}

#endif
