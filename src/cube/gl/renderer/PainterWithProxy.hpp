#ifndef  CUBE_GL_RENDERER_BINDINGS_PAINTERWITHPROXY_HPP
# define CUBE_GL_RENDERER_BINDINGS_PAINTERWITHPROXY_HPP

# include "fwd.hpp"
# include "Painter.hpp"
# include "Renderer.hpp"

# include <etc/log.hpp>

namespace cube { namespace gl { namespace renderer {

	///////////////////////////////////////////////////////////////////////
	// Painter proxy class.
	struct PainterWithProxy
	{
		ETC_LOG_COMPONENT("cube.gl.renderer.PainterWithProxy");

	private:
		Renderer&  _renderer;
		Mode       _mode;
		Painter*   _painter;

	public:
		PainterWithProxy(PainterWithProxy const& other)
			: _renderer(other._renderer)
			, _mode(other._mode)
			, _painter(other._painter)
		{}

		PainterWithProxy(Renderer& r, Mode const m)
			: _renderer(r)
			, _mode(m)
			, _painter(nullptr)
		{}

		Painter& __enter__()
		{
			assert(_painter == nullptr);
			ETC_TRACE.debug("Creating painter");
			_painter = new Painter(_renderer.begin(_mode));
			return *_painter;
		}

		void __exit__()
		{
			assert(_painter != nullptr);
			delete _painter;
			_painter = nullptr;
			ETC_TRACE.debug("Painter has been deleted");
		}
	};

}}}

#endif
