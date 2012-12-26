#include "PainterWithProxy.hpp"

#include "../renderer/Renderer.hpp"
#include "../renderer/Painter.hpp"

#include <etc/log.hpp>

namespace cube { namespace gl { namespace renderer_bindings {

	ETC_LOG_COMPONENT("cube.gl.renderer_bindings");

	PainterWithProxy::PainterWithProxy(PainterWithProxy const& other)
		: _renderer(other._renderer)
		, _mode(other._mode)
		, _painter(other._painter)
	{
	}

	PainterWithProxy::PainterWithProxy(renderer::Renderer& r, renderer::Mode const m)
		: _renderer(r)
		, _mode(m)
		, _painter(nullptr)
	{}

	renderer::Painter& PainterWithProxy::__enter__()
	{
		assert(_painter == nullptr);
		ETC_TRACE.debug("Creating painter");
		_painter = new renderer::Painter(_renderer.begin(_mode));
		return *_painter;
	}

	void PainterWithProxy::__exit__()
	{
		assert(_painter != nullptr);
		delete _painter;
		_painter = nullptr;
		ETC_TRACE.debug("Painter has been deleted");
	}

}}}
