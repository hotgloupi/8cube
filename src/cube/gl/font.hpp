#ifndef  CUBE_GL_FONT_HPP
# define CUBE_GL_FONT_HPP

# include "renderer/fwd.hpp"
# include "renderer/Bindable.hpp"

# include <etc/compiler.hpp>
# include <etc/types.hpp>

# include <iosfwd>
# include <list>
# include <memory>
# include <string>

namespace cube { namespace gl { namespace font {

# define CUBE_GL_FONT_DEFAULT_SIZE 12

	struct Infos;

	class Font
	{
	private:
		struct Impl;
		std::unique_ptr<Impl> _impl;

	public:
		explicit
		Font(renderer::Renderer& renderer,
		     Infos const& infos,
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
	 * @brief Retrieve the number of faces stored in one font file.
	 *
	 * @note This function will silently ignore any errors and return 0.
	 */
	etc::size_type
	get_faces_count(std::string const& path) ETC_NOEXCEPT;

	/**
	 * @brief Returns wether or not the file should be loadable.
	 */
	inline
	bool is_valid(std::string const& path) ETC_NOEXCEPT
	{
		return (get_faces_count(path) > 0);
	}

	/**
	 * @brief Retrieve infos for a specific face index in a font file.
	 *
	 * This function only return a valid Infos struct, or throws on any error.
	 */
	std::unique_ptr<Infos>
	get_face_infos(std::string const& path,
	               etc::size_type face_index = 0);

	/**
	 * @brief Retreive infos for a speficied font file.
	 *
	 * This function does return a valid list of Infos structures or throws on
	 * any error.
	 */
	std::list<std::unique_ptr<Infos>>
	get_infos(std::string const& path);

	/**
	 * @brief Font style.
	 */
	enum class Style
	{
		regular = 0,
		bold = 1,
		italic = 2,
		bold_italic = 3,
	};

	struct Infos
	{
	public:
		std::string const   path;
		std::string const   family_name;
		std::string const   style_name;
		Style const         style;
		bool const          has_horizontal;
		bool const          has_vertical;
		bool const          is_fixed_width;
		bool const          is_scalable;
		bool const          has_bitmaps;
		bool const          support_kerning;
		etc::size_type      face_index;

	public:
		/**
		 * @brief Retrieve the associated font.
		 */
		std::unique_ptr<Font>
		font(renderer::Renderer& renderer,
		     etc::size_type size = CUBE_GL_FONT_DEFAULT_SIZE);
		~Infos();

	private:
		Infos(std::string const& path,
		      std::string const& family_name,
		      std::string const& style_name,
		      Style const style,
		      bool const has_horizontal,
		      bool const has_vertical,
		      bool const is_fixed_width,
		      bool const is_scalable,
		      bool const has_fixed_sizes,
		      bool const support_kerning,
		      etc::size_type face_index);

		friend
		std::unique_ptr<Infos>
		get_face_infos(std::string const& path,
		               etc::size_type face_index);
	};

	/**
	 * @brief print operator on Font.
	 */
	std::ostream&
	operator <<(std::ostream& out, Infos const& rhs);

	/**
	 * @brief print operator on Style enum.
	 */
	std::ostream&
	operator <<(std::ostream& out, Style const rhs);

}}}

#endif
