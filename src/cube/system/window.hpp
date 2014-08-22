#ifndef  CUBE_SYSTEM_WINDOW_HPP
# define CUBE_SYSTEM_WINDOW_HPP

# include <cube/api.hpp>
# include <cube/gl/fwd.hpp>
# include <cube/gl/renderer/fwd.hpp>
# include <cube/system/fwd.hpp>

# include <string>
# include <memory>

namespace cube { namespace system { namespace window {

	class Window;
	class RendererContext;

	class CUBE_API Window
	{
	public:
		enum class Flags
		{
			none               = 0,
			borderless         = (1 << 0),
			fullscreen         = (1 << 1),
			fullscreen_desktop = (1 << 2),
			hidden             = (1 << 3),
		};
		typedef std::unique_ptr<RendererContext> RendererContextPtr;
		typedef std::unique_ptr<cube::gl::renderer::Renderer> RendererPtr;
		typedef std::unique_ptr<inputs::Inputs> InputsPtr;

	protected:
		struct Impl;
		typedef std::unique_ptr<Impl> ImplPtr;
		ImplPtr _impl;

	public:
		gl::renderer::Renderer& renderer();
		RendererContext& renderer_context() const;
		inputs::Inputs& inputs();
		std::string const& title() const ETC_NOEXCEPT;
		void title(std::string const& title);
		etc::size_type width() const ETC_NOEXCEPT;
		etc::size_type height() const ETC_NOEXCEPT;
		Flags flags() const ETC_NOEXCEPT;

	protected:
		void size(etc::size_type const width,
		          etc::size_type const height);
		virtual
		void _title(std::string const& title) = 0;

	protected:
		Window(ImplPtr&& impl) ETC_NOEXCEPT;
	public:
		static
		std::unique_ptr<Window>
		create(std::string title,
		       etc::size_type const width,
		       etc::size_type const height,
		       Flags const flags = Flags::none,
		       gl::renderer::Name const name  = gl::renderer::Name::OpenGL);

		static
		std::unique_ptr<Window>
		create(std::string title,
		       RendererContextPtr renderer_context);
	public:
		static
		RendererContextPtr
		create_renderer_context(etc::size_type const width,
		                        etc::size_type const height,
		                        Flags const flags,
		                        gl::renderer::Name const name);

	public:
		virtual ~Window();

	public:
		etc::size_type poll();
		etc::size_type poll(etc::size_type const max);
	protected:
		virtual
		etc::size_type _poll(etc::size_type const max) = 0;

	public:
		virtual
		void confine_mouse(bool mode) = 0;

		virtual
		void swap_buffers() = 0;

		virtual
		void show() = 0;

		virtual
		void raise() = 0;

		virtual
		void hide() = 0;

		virtual
		void minimize() = 0;

		virtual
		void maximize() = 0;

		virtual
		void screenshot(std::string const& file) const = 0;

		virtual
		gl::vector::Vector2i mouse_position() const = 0;
	};

	class CUBE_API RendererContext
	{
	protected:
		etc::size_type           _width;
		etc::size_type           _height;
		Window::Flags            _flags;
		gl::renderer::Name const _name;

	protected:
		RendererContext(etc::size_type const width,
		                etc::size_type const height,
		                Window::Flags const flags,
		                gl::renderer::Name const name) ETC_NOEXCEPT;


	public:
		virtual
		etc::size_type width() const ETC_NOEXCEPT;

		virtual
		etc::size_type height() const ETC_NOEXCEPT;

		virtual
		Window::Flags flags() const ETC_NOEXCEPT;

		gl::renderer::Name name() const ETC_NOEXCEPT;

	public:
		void size(etc::size_type const width, etc::size_type const height);
	protected:
		virtual
		void _size(etc::size_type const width,
		           etc::size_type const height) = 0;


	public:
		inline
		gl::renderer::RenderTargetPtr new_render_target()
		{ return this->new_render_target(this->width(), this->height()); }

		virtual
		gl::renderer::RenderTargetPtr
		new_render_target(etc::size_type const width,
		                  etc::size_type const height) = 0;

	public:
		virtual
		~RendererContext();
	};

	inline
	Window::Flags operator |(Window::Flags const lhs,
	                         Window::Flags const rhs) ETC_NOEXCEPT
	{ return (Window::Flags) (((int) lhs) | ((int) rhs)); }

	inline
	bool operator &(Window::Flags const lhs, Window::Flags const rhs) ETC_NOEXCEPT
	{ return (bool) (((int) lhs) & ((int) rhs)); }

}}} // !cube::system::window

#endif
