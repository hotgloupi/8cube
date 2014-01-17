#ifndef  CUBE_GL_TEXT_HPP
# define CUBE_GL_TEXT_HPP

# include "fwd.hpp"
# include "renderer/fwd.hpp"

# include <cube/api.hpp>

# include <etc/types.hpp>

# include <boost/noncopyable.hpp>

# include <string>

namespace cube { namespace gl { namespace text {

	class CUBE_API Text
		: private boost::noncopyable
	{
	private:
		font::Font&               _font;
		etc::size_type            _size;
		renderer::VertexBufferPtr _vertices;
		material::MaterialPtr     _material;
		renderer::BindablePtr     _material_view;

	public:
		template<typename CharType>
		Text(font::Font& font,
		     std::basic_string<CharType> const& str);
		template<typename CharType>
		Text(font::Font& font, CharType const* str)
			: Text(font, std::basic_string<CharType>(str != nullptr ? str : "(nil)"))
		{}
		~Text();

		void draw(renderer::Painter& painter);

		void color(color::Color3f const& color);
		color::Color3f const& color() const;
	};

}}}

#endif
