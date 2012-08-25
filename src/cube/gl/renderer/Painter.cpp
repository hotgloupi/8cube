#include "Painter.hpp"

#include "Drawable.hpp"
#include "Renderer.hpp"
#include "State.hpp"
#include "VertexBuffer.hpp"

namespace cube { namespace gl { namespace renderer {

	//////////////////////////////////////////////////////////////////////////
	// Painter class
	Painter::Painter(Renderer& renderer)
		: _renderer(renderer)
		, _current_state(_renderer.current_state())
	{
		_renderer.current_state().painter(this);
	}

	Painter::Painter(Painter&& other)
		: _renderer(other._renderer)
		, _current_state(other._current_state)
		, _bound_drawables(std::move(other._bound_drawables))
	{
		_current_state.painter_switch(&other, this);
	}

	Painter::~Painter()
	{
		for (Drawable* drawable: _bound_drawables)
			drawable->_unbind();
		_bound_drawables.clear();

		_renderer._pop_state();
	}

	void Painter::bind(Drawable& drawable)
	{
		if (_bound_drawables.find(&drawable) != _bound_drawables.end())
			throw Exception{"Already bound drawable"};
		drawable._bind();
		_bound_drawables.insert(&drawable);
	}

	void Painter::update(MatrixKind kind, matrix_type const& matrix)
	{
		for (auto& drawable : _bound_drawables)
		{
			drawable->update(kind, matrix);
		}
	}

	void Painter::draw_elements(DrawMode mode,
	                            VertexBuffer& indices,
	                            unsigned int start,
	                            unsigned int count)
	{
		if (indices.attributes().size() == 0)
			throw Exception{
				"No attributes found in indices VertexBuffer."
			};
		if (indices.attributes().size() > 1)
			throw Exception{
				"Indices VertexBuffer contains more that one attributes."
			};

		auto const& attr = indices.attributes()[0];

		if (start > attr.nb_elements)
			throw Exception{
				"Start index is out of range"
			};
		else if (start == attr.nb_elements)
			throw Exception{
				"Start index equals the number of elements:"
				" nothing would has been rendered."
			};

		if (count == ((unsigned int) -1))
			count = attr.nb_elements - start;
		else if (count > attr.nb_elements - start)
			throw Exception{"Count is out of range."};


		this->bind(indices);
		_renderer.draw_elements(
			mode,
			count,
			attr.type,
			(uint8_t*)0 + (start * get_content_type_size(attr.type))
		);
		this->unbind(indices);
	}


	void Painter::unbind(Drawable& drawable)
	{
		auto it = _bound_drawables.find(&drawable);
		if (it == _bound_drawables.end())
			throw Exception{"Cannot unbind the drawable (not found)"};
		drawable._unbind();
		_bound_drawables.erase(it);
	}

}}} //!cube::gl::renderer
