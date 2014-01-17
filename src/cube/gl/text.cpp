#include "text.hpp"

#include "font.hpp"

#include <cube/gl/color.hpp>
#include <cube/gl/material.hpp>
#include <cube/gl/renderer/Painter.hpp>
#include <cube/gl/renderer/Renderer.hpp>
#include <cube/gl/renderer/ShaderProgram.hpp>
#include <cube/gl/renderer/VertexBuffer.hpp>
#include <cube/gl/renderer/Texture.hpp>

namespace cube { namespace gl { namespace text {

	template<typename CharType>
	Text::Text(font::Font& font,
	           std::basic_string<CharType> const& str)
		: _font(font)
		, _size(str.size())
		, _vertices{_font.generate_text(str)}
		, _material{new material::Material}
		, _material_view{nullptr}
	{
		_material->add_texture(
		    _font.texture(),
		    material::TextureType::opacity,
		    material::TextureMapping::uv,
		    material::StackOperation::add,
		    material::TextureMapMode::wrap,
		    1.0// material::BlendMode::basic
		);
		_material->ambient(color::Color3f("white"));
	}

	template
	CUBE_API
	Text::Text/*<char>*/(font::Font&, std::basic_string<char> const&);
	//template
	//CUBE_API
	//Text::Text/*<wchar_t>*/(font::Font&, std::basic_string<wchar_t> const&);
	//template
	//CUBE_API
	//Text::Text/*<char16_t>*/(font::Font&, std::basic_string<char16_t> const&);
	//template
	//CUBE_API
	//Text::Text/*<char32_t>*/(font::Font&, std::basic_string<char32_t> const&);

	Text::~Text()
	{}

	void Text::draw(renderer::Painter& painter)
	{
		if (_material_view == nullptr)
			_material_view = _material->bindable(_font.renderer());

		painter.with(*_material_view)
			->draw_arrays(renderer::DrawMode::quads, *_vertices);
	}

	color::Color3f const& Text::color() const
	{ return _material->ambient(); }

	void Text::color(color::Color3f const& color)
	{
		_material->ambient(color);
		_material_view = nullptr;
	}

}}}
