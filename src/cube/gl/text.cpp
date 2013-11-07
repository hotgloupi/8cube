#include "text.hpp"

#include "font.hpp"
#include "renderer/Painter.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/ShaderProgram.hpp"
#include "renderer/VertexBuffer.hpp"
#include "renderer/Texture.hpp"

namespace cube { namespace gl { namespace text {

	template<typename CharType>
	Text::Text(font::Font& font,
	           std::basic_string<CharType> const& str)
		: _font(font)
		, _size(str.size())
		, _vertices{_font.generate_text(str)}
	{}

	template
	CUBE_API
	Text::Text/*<char>*/(font::Font&, std::basic_string<char> const&);
	template
	CUBE_API
	Text::Text/*<wchar_t>*/(font::Font&, std::basic_string<wchar_t> const&);
	template
	CUBE_API
	Text::Text/*<char16_t>*/(font::Font&, std::basic_string<char16_t> const&);
	template
	CUBE_API
	Text::Text/*<char32_t>*/(font::Font&, std::basic_string<char32_t> const&);

	Text::~Text()
	{}

	void Text::draw(renderer::Painter& painter,
	                renderer::ShaderProgramParameter& sampler)
	{
		auto proxy = painter.with(_font.texture());
		sampler = _font.texture();
		proxy->draw_arrays(renderer::DrawMode::quads, *_vertices);
	}

}}}
