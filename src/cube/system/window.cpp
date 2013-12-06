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
		RendererContextPtr context;
		InputsPtr          inputs;
		RendererPtr        renderer;

		Impl(std::string title,
		     RendererContextPtr context,
		     InputsPtr inputs)
			: title{std::move(title)}
			, context{std::move(context)}
			, inputs{std::move(inputs)}
			, renderer{gl::renderer::create_renderer(*this->context)}
		{}
	};

	Window::Window(ImplPtr&& impl) noexcept
		: _impl{std::move(impl)}
	{
		ETC_TRACE_CTOR(_impl->title, this->width(), this->height());
	}

	Window::~Window()
	{
		ETC_TRACE_DTOR(_impl->title);
		_impl->renderer->shutdown();
	}

	etc::size_type Window::poll()
	{ return this->poll(100); }

	etc::size_type Window::poll(etc::size_type const max)
	{ return _poll(max); }

	gl::renderer::Renderer& Window::renderer()
	{ return *_impl->renderer; }

	inputs::Inputs& Window::inputs()
	{ return *_impl->inputs; }

	RendererContext& Window::renderer_context() const
	{ return *_impl->context; }

	etc::size_type Window::width() const noexcept
	{ return _impl->context->width(); }

	etc::size_type Window::height() const noexcept
	{ return _impl->context->height(); }

	Window::Flags Window::flags() const noexcept
	{ return _impl->context->flags(); }

	std::string const& Window::title() const noexcept
	{ return _impl->title; }

	void Window::title(std::string const& title)
	{
		_title(title);
		_impl->title = title;
	}

	void Window::size(etc::size_type const w,
	                  etc::size_type const h)
	{ _impl->context->size(w, h); }


	std::unique_ptr<Window>
	Window::create(std::string title,
	               RendererContextPtr renderer_context)
	{
		auto impl_ptr = std::unique_ptr<Window::Impl>{
			new Impl{
				std::move(title),
				std::move(renderer_context),
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

	std::unique_ptr<Window>
	Window::create(std::string title,
	               etc::size_type const width,
	               etc::size_type const height,
	               Flags const flags,
	               gl::renderer::Name const name)
	{
		return create(
			std::move(title),
			CUBE_SYSTEM_WINDOW_TYPE::create_renderer_context(
				width,
				height,
				flags,
				name
			)
		);
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

	RendererContext::RendererContext(etc::size_type const width,
	                                 etc::size_type const height,
	                                 Window::Flags const flags,
	                                 gl::renderer::Name const name) noexcept
		: _width{width}
		, _height{height}
		, _flags{flags}
		, _name{name}
	{}

	etc::size_type RendererContext::width() const noexcept
	{ return _width; }

	etc::size_type RendererContext::height() const noexcept
	{ return _height; }

	Window::Flags RendererContext::flags() const noexcept
	{ return _flags; }

	gl::renderer::Name RendererContext::name() const noexcept
	{ return _name; }

	void
	RendererContext::size(etc::size_type const width,
	                      etc::size_type const height)
	{
		_size(width, height);
		_width = width;
		_height = height;
	}

	RendererContext::~RendererContext()
	{
		ETC_LOG.debug(this, "Deleting renderer context");
	}

}}} // !cube::system::window
