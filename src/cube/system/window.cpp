#include <wrappers/sdl.hpp>

#include <etc/log.hpp>

#include <cube/gl/renderer.hpp>

#include "Exception.hpp"
#include "inputs.hpp"
#include "window.hpp"

#include "sdl/window.hpp"

#ifdef CUBE_WITH_GLX
# include "glx/window.hpp"
#endif

namespace cube { namespace system { namespace window {

	ETC_LOG_COMPONENT("cube.system.window.Window");

	struct Window::Impl
		: private boost::noncopyable
	{
	public:
		typedef std::unique_ptr<gl::renderer::Renderer> RendererPtr;

	public:
		RendererPtr     renderer;
		inputs::Inputs  inputs;

	public:
		Impl(gl::renderer::Name const name)
			: renderer{}
			, inputs{}
		{
			ETC_LOG.debug("SDL has been initialized, creating the renderer");
			// SDL_SetVideoMode has been called before (with resize()), we can
			// create the renderer safely.
			//this->renderer = std::move(gl::renderer::create_renderer(vp, name));
		}

		~Impl()
		{
			ETC_LOG.debug("Deleting Window impl");
			this->renderer.reset();
		}
	};

	Window::Window(std::string const& title,
	               etc::size_type const width,
	               etc::size_type const height,
	               gl::renderer::Name const renderer_name)
		: _impl(nullptr)
		, _title(title)
		, _width(width)
		, _height(height)
	{
		ETC_LOG.debug("New window", title, width, height);
		_impl.reset(new Impl{renderer_name});
	}

	Window::~Window()
	{
		ETC_LOG.debug("Deleting window", _title, _width, _height);
		if (_impl->renderer)
			_impl->renderer->shutdown();
	}

	etc::size_type Window::poll()
	{
		return this->poll(100);
	}

	gl::renderer::Renderer& Window::renderer()
	{
		if (!_impl->renderer)
			throw Exception{"No renderer has been created yet"};
		return *_impl->renderer;
	}

	inputs::Inputs& Window::inputs()
	{
		return _impl->inputs;
	}

	std::unique_ptr<Window>
	create_window(std::string const& title,
	              etc::size_type const width,
	              etc::size_type const height,
	              gl::renderer::Name const name)
	{
		auto ptr =  std::unique_ptr<Window>{
			new sdl::window::Window{title, width, height, name}
		};
		gl::viewport::Viewport vp{0, 0, (float)width, (float)height};
		ptr->_impl->renderer = gl::renderer::create_renderer(vp, name);
		ETC_LOG.debug("Successfully created window and renderer");
		return ptr;
	}

}}} // !cube::system::window
