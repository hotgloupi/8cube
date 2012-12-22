#ifndef  CUBE_GL_TEXT_HPP
# define CUBE_GL_TEXT_HPP

# include "fwd.hpp"
# include "renderer/Drawable.hpp"

# include <etc/types.hpp>

# include <boost/noncopyable.hpp>

# include <string>

namespace cube { namespace gl { namespace text {

	class Text
		: public renderer::Drawable<renderer::ShaderProgramParameter>
		, private boost::noncopyable
	{
	private:
		font::Font&             _font;
		etc::size_type          _size;
		renderer::VertexBuffer* _vertices;

	public:
		template<typename CharType>
		Text(font::Font& font,
		     std::basic_string<CharType> const& str);
		template<typename CharType>
		Text(font::Font& font, CharType const* str)
			: Text(font, std::basic_string<CharType>(str))
		{}
		~Text();
	private:
		void _draw(renderer::Painter& painter,
		           renderer::ShaderProgramParameter& sampler);
	};

}}}

#endif
