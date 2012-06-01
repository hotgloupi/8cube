#include "renderer.hpp"

#include <iostream>
#include <stdexcept>

#include <wrappers/boost/python.hpp>

#include "opengl/Renderer.hpp"


namespace cube { namespace gl { namespace renderer {

	std::vector<cube::gl::renderer::Renderer*> const& all_renderers()
	{
		static ::cube::gl::opengl::GLRenderer opengl_renderer;

		static std::vector<Renderer*> renderers{{
			&opengl_renderer,
		}};
		return renderers;
	}

	RendererType const& default_renderer_type = all_renderers()[0]->description();

	std::unique_ptr<Renderer>
		create_renderer(cube::gl::viewport::Viewport const& vp,
		                RendererType const& description)
		{
			return description.create(vp);
		}

	Renderer::Renderer()
		: _viewport{0,0,0,0}
		, _states{}
	{
		State default_state{
			Mode::none,
			matrix_type{},
			matrix_type{},
			matrix_type{},
			matrix_type{},
		};
		_states.push_back(default_state);
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::viewport(cube::gl::viewport::Viewport const& vp)
	{
		std::cout << "Setting viewport("
		          << vp.x << ", "
		          << vp.y << ", "
		          << vp.w << ", "
		          << vp.h << ")\n";
		_viewport = vp;
	}

	void Renderer::push_state(State const& state)
	{
		this->_states.push_back(state);
	}

	void Renderer::pop_state()
	{
		_states.pop_back();
	}

	Renderer::State const& Renderer::current_state() const
	{
		return _states.back();
	}

	Renderer::Painter Renderer::begin(State const& state)
	{
		_states.push_back(state);
		return Painter(*this);
	}

}}} // !cube::gl::renderer

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

}}}} // !cube::gl::renderer::detail
