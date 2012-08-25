#include "renderer.hpp"
#include "renderer/Exception.hpp"
#include "renderer/VertexBuffer.hpp"
//#include "renderer/Shader.hpp"
//#include "renderer/ShaderProgram.hpp"

#include <iostream>
#include <stdexcept>

#include <wrappers/boost/python.hpp>

#include <etc/print.hpp>

#include "opengl/Renderer.hpp"


namespace cube { namespace gl { namespace renderer {

	//////////////////////////////////////////////////////////////////////////
	// free functions

	std::vector<cube::gl::renderer::Renderer*> const&
	all_renderers()
	{
		static ::cube::gl::opengl::GLRenderer opengl_renderer;

		static std::vector<Renderer*> renderers{{
			&opengl_renderer,
		}};
		return renderers;
	}

	std::unique_ptr<Renderer>
	create_renderer(cube::gl::viewport::Viewport const& vp,
	                RendererType::Name name)
	{
		for (auto const& renderer: all_renderers())
		{
			if (renderer->description().name() == name)
				return renderer->description().create(vp);
		}
		throw Exception{"Cannot find any renderer with that name"};
	}

	///////////////////////////////////////////////////////////////////////////
	// RendererType

	std::ostream& operator <<(std::ostream& out, RendererType const& descr)
	{
		return (out << descr.__str__());
	}

	///////////////////////////////////////////////////////////////////////////
	// Renderer class

	Renderer::Renderer()
		: _viewport{0,0,0,0}
		, _states{}
	{
		_states.push_back(State(
			Mode::none,
			matrix_type{},
			matrix_type{},
			matrix_type{}
		));
	}

	Renderer::~Renderer()
	{
	}

	void
	Renderer::update_projection_matrix()
	{
		switch (this->current_state().mode)
		{
		case Mode::none:
			break;
		case Mode::_2d:
			this->current_state().projection(matrix::ortho<float>(
				_viewport.x, _viewport.w + _viewport.x,
				_viewport.y + _viewport.h, _viewport.y
			));
			break;
		case Mode::_3d:
			throw Exception{"Not implemented."};
			break;
		default:
			throw Exception{"Unknown render mode."};
		}
	}

	void Renderer::viewport(cube::gl::viewport::Viewport const& vp)
	{
		_viewport = vp;
		this->update_projection_matrix();
	}

	void Renderer::push_state(State const& state)
	{
		_states.push_back(state);
	}

	void Renderer::pop_state()
	{
		_states.pop_back();
	}

	//////////////////////////////////////////////////////////////////////////
	// Painter class
	Renderer::Painter::Painter(Renderer& renderer)
		: _renderer(renderer)
		, _current_state(_renderer.current_state())
	{
		_renderer.current_state().painter(this);
	}

	Renderer::Painter::Painter(Painter&& other)
		: _renderer(other._renderer)
		, _current_state(other._current_state)
		, _bound_drawables(std::move(other._bound_drawables))
	{
		_renderer.current_state().painter_switch(&other, this);
	}

	Renderer::Painter::~Painter()
	{
		for (Drawable* drawable: _bound_drawables)
			drawable->_unbind();
		_bound_drawables.clear();

		_renderer.pop_state();
	}

	void Renderer::Painter::bind(Drawable& drawable)
	{
		if (_bound_drawables.find(&drawable) != _bound_drawables.end())
			throw Exception{"Already bound drawable"};
		drawable._bind();
		_bound_drawables.insert(&drawable);
	}

	Renderer::Painter Renderer::begin(State const& state)
	{
		switch (state.mode)
		{
		case Mode::none:
			throw Exception{"Cannot begin rendering in Mode::none."};
		case Mode::_2d:
		case Mode::_3d:
			break;
		default:
			throw Exception{"Unknown render mode."};
		}
		_states.push_back(state);
		this->update_projection_matrix();
		return Painter(*this);
	}

	void Renderer::Painter::draw_elements(DrawMode mode,
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


	void Renderer::Painter::unbind(Drawable& drawable)
	{
		auto it = _bound_drawables.find(&drawable);
		if (it == _bound_drawables.end())
			throw Exception{"Cannot unbind the drawable (not found)"};
		drawable._unbind();
		_bound_drawables.erase(it);
	}

}}} // !cube::gl::renderer

//////////////////////////////////////////////////////////////////////////////
// Python wrappers

namespace cube { namespace gl { namespace renderer { namespace detail {

	std::string WrapRendererType::__str__() const
	{
		return this->renderer_type->__str__();
	}

	PainterWithProxy::PainterWithProxy(PainterWithProxy const& other)
		: _renderer(other._renderer)
		, _mode(other._mode)
		, _painter(other._painter)
	{
	}

	PainterWithProxy::PainterWithProxy(Renderer& r, Renderer::Mode m)
		: _renderer(r)
		, _mode(m)
		, _painter(nullptr)
	{}

	Renderer::Painter& PainterWithProxy::__enter__()
	{
		assert(_painter == nullptr);
		_painter = new Renderer::Painter(_renderer.begin(_mode));
		return *_painter;
	}

	void PainterWithProxy::__exit__()
	{
		assert(_painter != nullptr);
		delete _painter;
		_painter = nullptr;
	}

	WrapRenderer::WrapRenderer()
		: _renderer(nullptr)
	{}

	WrapRenderer::WrapRenderer(Renderer& r)
		: _renderer(&r)
	{}

	WrapRenderer::WrapRenderer(WrapRenderer const& other)
		: _renderer(other._renderer)
	{}

	WrapRendererType WrapRenderer::description() const
	{
		assert(_renderer != nullptr);
		return WrapRendererType{&_renderer->description()};
	}

	void WrapRenderer::swap_buffers()
	{
		assert(_renderer != nullptr);
		_renderer->swap_buffers();
	}

	void WrapRenderer::viewport(float x, float y, float w, float h)
	{
		assert(_renderer != nullptr);
		_renderer->viewport(
			cube::gl::viewport::Viewport{x, y, w, h}
		);
	}

	PainterWithProxy WrapRenderer::begin(Renderer::Mode mode)
	{
		return PainterWithProxy(*_renderer, mode);
	}

	VertexBuffer* WrapRenderer::new_vertex_buffer()
	{
		return _renderer->new_vertex_buffer().release();
	}

	VertexBuffer* WrapRenderer::new_index_buffer()
	{
		return _renderer->new_index_buffer().release();
	}

	void WrapRenderer::clear(int flags)
	{
		return _renderer->clear((BufferBit)flags);
	}

}}}} // !cube::gl::renderer::detail
