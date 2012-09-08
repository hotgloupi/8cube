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
		, _indices{nullptr}
	{
		_indices = _font.generate_text(str).release();
	}

	template
	Text::Text<char>(font::Font&, std::basic_string<char> const&);
	template
	Text::Text<wchar_t>(font::Font&, std::basic_string<wchar_t> const&);

	Text::~Text()
	{
		delete _indices;
		_indices = nullptr;
	}

	void Text::render(renderer::Painter& painter,
	                  renderer::ShaderProgramParameter* param)
	{
		_font.bind(painter);
		painter.draw_elements(
			renderer::DrawMode::triangle_strip,
			*_indices,
			0,
			_size * 4
		);
	}

}}}
