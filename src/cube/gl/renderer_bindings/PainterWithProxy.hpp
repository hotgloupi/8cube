#ifndef  CUBE_GL_RENDERER_BINDINGS_PAINTERWITHPROXY_HPP
# define CUBE_GL_RENDERER_BINDINGS_PAINTERWITHPROXY_HPP

# include "../renderer/fwd.hpp"

namespace cube { namespace gl { namespace renderer_bindings {

	///////////////////////////////////////////////////////////////////////
	// Painter proxy class.
	struct PainterWithProxy
	{
	private:
		renderer::Renderer&  _renderer;
		renderer::Mode       _mode;
		renderer::Painter*   _painter;

	public:
		PainterWithProxy(PainterWithProxy const& other);
		PainterWithProxy(renderer::Renderer& r, renderer::Mode const m);
		renderer::Painter& __enter__();
		void __exit__();
	};

}}}

#endif
