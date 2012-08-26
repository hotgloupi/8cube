#include "Renderer.hpp"

#include "Exception.hpp"
#include "Painter.hpp"
#include "State.hpp"
#include "VertexBuffer.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"

#include <etc/log.hpp>

namespace cube { namespace gl { namespace renderer {

	static auto& log = etc::log::logger("cube.gl.renderer.Renderer");

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
		_push_state(State(Mode::none));
	}

	Renderer::~Renderer()
	{
		assert(_states.back().mode == Mode::none);
		_pop_state();
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

	Painter Renderer::begin(State&& state)
	{
		log.debug("Begining new state in mode", state.mode);
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
		_push_state(std::move(state));
		Painter p(*this);
		this->update_projection_matrix();
		return p;//Painter(*this);
	}

	void Renderer::viewport(cube::gl::viewport::Viewport const& vp)
	{
		_viewport = vp;
		this->update_projection_matrix();
	}

	void Renderer::_push_state(State&& state)
	{
		_states.push_back(std::move(state));
	}

	void Renderer::_pop_state()
	{
		_states.pop_back();
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

	PainterWithProxy::PainterWithProxy(Renderer& r, Mode m)
		: _renderer(r)
		, _mode(m)
		, _painter(nullptr)
	{}

	Painter& PainterWithProxy::__enter__()
	{
		assert(_painter == nullptr);
		log.debug("Creating painter");
		_painter = new Painter(_renderer.begin(_mode));
		return *_painter;
	}

	void PainterWithProxy::__exit__()
	{
		assert(_painter != nullptr);
		delete _painter;
		_painter = nullptr;
		log.debug("Painter has been deleted");
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

	PainterWithProxy WrapRenderer::begin(Mode mode)
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

	ShaderProgram* WrapRenderer::new_shader_program()
	{
		return _renderer->new_shader_program().release();
	}

	Shader* WrapRenderer::new_vertex_shader()
	{
		return _renderer->new_vertex_shader().release();
	}

	Shader* WrapRenderer::new_fragment_shader()
	{
		return _renderer->new_fragment_shader().release();
	}

	void WrapRenderer::clear(int flags)
	{
		return _renderer->clear((BufferBit)flags);
	}

}}}} // !cube::gl::renderer::detail
