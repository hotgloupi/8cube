#include "text.hpp"

#include "font.hpp"
#include "renderer/Painter.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/VertexBuffer.hpp"

namespace cube { namespace gl { namespace text {

	template<typename CharType>
	Text::Text(font::Font& font,
	           std::basic_string<CharType> const& str)
		: _font(font)
		, _size(str.size())
		, _vertices{nullptr}
	{
		_vertices = _font.generate_text(str).release();
	}

	template
	Text::Text<char>(font::Font&, std::basic_string<char> const&);
	template
	Text::Text<wchar_t>(font::Font&, std::basic_string<wchar_t> const&);
	template
	Text::Text<char16_t>(font::Font&, std::basic_string<char16_t> const&);
	template
	Text::Text<char32_t>(font::Font&, std::basic_string<char32_t> const&);

	Text::~Text()
	{
		delete _vertices;
		_vertices = nullptr;
	}

	void Text::_draw(renderer::Painter& painter,
	                 renderer::ShaderProgramParameter& sampler)
	{
		_font.bind(painter, sampler);
		painter.draw_arrays(renderer::DrawMode::quads, *_vertices);
	}

}}}
