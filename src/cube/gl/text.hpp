#ifndef  CUBE_GL_TEXT_HPP
# define CUBE_GL_TEXT_HPP

# include "fwd.hpp"
# include "renderer/fwd.hpp"

# include <etc/types.hpp>

# include <string>

namespace cube { namespace gl { namespace text {

	class Text
	{
	private:
		font::Font&             _font;
		etc::size_type          _size;
		renderer::VertexBuffer* _indices;

	public:
		template<typename CharType>
		Text(font::Font& font,
		     std::basic_string<CharType> const& str);
		Text(font::Font& font, char const* str)
			: Text(font, std::string{str})
		{}
		void render(renderer::Painter& painter,
		            renderer::ShaderProgramParameter& param);
		~Text();
	};

}}}

#endif

