#ifndef  CUBE_GL_FONT_HPP
# define CUBE_GL_FONT_HPP

# include "renderer/fwd.hpp"
# include "renderer/Bindable.hpp"

# include <cube/api.hpp>
# include <cube/resource/Resource.hpp>

# include <etc/compiler.hpp>
# include <etc/types.hpp>

# include <iosfwd>
# include <list>
# include <memory>
# include <string>

namespace cube { namespace gl { namespace font {

# define CUBE_GL_FONT_DEFAULT_SIZE 12

	struct Infos;

	class CUBE_API Font
		: public resource::Resource
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
		 * @brief Generate a vertex buffer corresponding to a string.
		 *
		 * The vertex buffer returned is filled with vertices and font texture
		 * coordinates.
		 */
		//template<typename CharType>
		renderer::VertexBufferPtr
		generate_text(std::string const& str);

		/**
		 * @brief The texture of the font.
		 *
		 * The texture is generated according the needs of previous calls to
		 * generate_text(), and will be updated depending on characters needed.
		 *
		 * @warning You should not call generate_text while the texture is
		 *          bound. however, you can safely keep a reference to it, it
		 *          will be updated inplace.
		 */
		renderer::TexturePtr& texture();
	};


	/**
	 * @brief Retrieve the number of faces stored in one font file.
	 *
	 * @note This function will silently ignore any errors and return 0.
	 */
	CUBE_API
	etc::size_type
	get_faces_count(std::string const& path) ETC_NOEXCEPT;

	/**
	 * @brief Returns wether or not the file should be loadable.
	 */
	CUBE_API
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
	CUBE_API
	std::unique_ptr<Infos>
	get_face_infos(std::string const& path,
	               etc::size_type face_index = 0);

	/**
	 * @brief Retreive infos for a speficied font file.
	 *
	 * This function does return a valid list of Infos structures or throws on
	 * any error.
	 */
	CUBE_API
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

	struct CUBE_API Infos
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

		/**
		 * @brief Construct a font with full information.
		 *
		 * Infos should not be constructed directly, instead you should use @a
		 * `get_infos()` or @a `get_face_infos()`.
		 *
		 * @note It is exposed to enable pickling from python.
		 */
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
		CUBE_API
		std::unique_ptr<Infos>
		get_face_infos(std::string const& path,
		               etc::size_type face_index);
	};

	/**
	 * @brief print operator on Font.
	 */
	CUBE_API
	std::ostream&
	operator <<(std::ostream& out, Infos const& rhs);

	/**
	 * @brief print operator on Style enum.
	 */
	CUBE_API
	std::ostream&
	operator <<(std::ostream& out, Style const rhs);

}}}

#endif
