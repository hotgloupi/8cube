#include <wrappers/sdl.hpp>

#include <etc/log.hpp>

#include <cube/gl/renderer.hpp>

#include "Exception.hpp"
#include "inputs.hpp"
#include "window.hpp"

#include "sdl/window.hpp"
#define CUBE_SYSTEM_WINDOW_TYPE cube::system::sdl::window::Window

#ifdef CUBE_WITH_GLX
# include "glx/window.hpp"
#endif

namespace cube { namespace system { namespace window {

	ETC_LOG_COMPONENT("cube.system.window.Window");

	struct Window::Impl
	{
		std::string        title;
		etc::size_type     width;
		etc::size_type     height;
		Flags              flags;
		RendererContextPtr context;
		InputsPtr          inputs;
		RendererPtr        renderer;

		Impl(std::string const& title,
		     etc::size_type const width,
		     etc::size_type const height,
		     Flags const flags,
		     RendererContextPtr context,
		     InputsPtr inputs)
			: title{title}
			, width{width}
			, height{height}
			, flags{flags}
			, context{std::move(context)}
			, inputs{std::move(inputs)}
			, renderer{
				gl::renderer::create_renderer(
					gl::viewport::Viewport{0, 0, (float) width, (float) height},
					cube::gl::renderer::Name::OpenGL
				)
			}
		{}
	};

	Window::Window(ImplPtr&& impl) noexcept
		: _impl{std::move(impl)}
	{
		ETC_LOG.debug("New window", _impl->title, _impl->width, _impl->height);
	}

	Window::~Window()
	{
		ETC_LOG.debug("Deleting window", _impl->title);
		_impl->renderer->shutdown();
	}

	etc::size_type Window::poll()
	{
		return this->poll(100);
	}

	gl::renderer::Renderer& Window::renderer()
	{ return *_impl->renderer; }

	inputs::Inputs& Window::inputs()
	{ return *_impl->inputs; }

	RendererContext& Window::renderer_context()
	{ return *_impl->context; }

	etc::size_type Window::width() const noexcept
	{ return _impl->width; }

	etc::size_type Window::height() const noexcept
	{ return _impl->height; }

	Window::Flags Window::flags() const noexcept
	{ return _impl->flags; }

	std::string const& Window::title() const noexcept
	{ return _impl->title; }

	void Window::width(etc::size_type const w) noexcept
	{ _impl->width = w; }

	void Window::height(etc::size_type const h) noexcept
	{ _impl->height = h; }

	std::unique_ptr<Window>
	Window::create(std::string const& title,
	               etc::size_type const width,
	               etc::size_type const height,
	               Flags const flags,
	               gl::renderer::Name const name)
	{
		auto impl_ptr = std::unique_ptr<Window::Impl>{
			new Impl{
				title,
				width,
				height,
				flags,
				CUBE_SYSTEM_WINDOW_TYPE::create_renderer_context(
					width,
					height,
					flags,
					name
				),
				etc::make_unique<inputs::Inputs>(),
			}
		};
		auto ptr =  std::unique_ptr<Window>{
			new CUBE_SYSTEM_WINDOW_TYPE{
				std::move(impl_ptr)
			}
		};
		ETC_LOG.debug("Successfully created window and renderer");
		return ptr;
	}

	Window::RendererContextPtr
	Window::create_renderer_context(etc::size_type const width,
	                                etc::size_type const height,
	                                Flags const flags,
	                                gl::renderer::Name const name)
	{
		return CUBE_SYSTEM_WINDOW_TYPE::create_renderer_context(
			width,
			height,
			flags | Flags::hidden,
			name
		);
	}

	RendererContext::~RendererContext()
	{
		ETC_LOG.debug(this, "Deleting renderer context");
	}

}}} // !cube::system::window
