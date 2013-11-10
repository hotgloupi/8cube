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
		RendererContext& renderer_context();
		inputs::Inputs& inputs();
		std::string const& title() const noexcept;
		etc::size_type width() const noexcept;
		etc::size_type height() const noexcept;
		Flags flags() const noexcept;

	protected:
		void width(etc::size_type const w) noexcept;
		void height(etc::size_type const h) noexcept;

	protected:
		Window(ImplPtr&& impl) noexcept;
	public:
		static
		std::unique_ptr<Window>
		create(std::string const& title,
		       etc::size_type const width,
		       etc::size_type const height,
		       Flags const flags = Flags::none,
		       gl::renderer::Name const name  = gl::renderer::Name::OpenGL);

	public:
		static
		RendererContextPtr
		create_renderer_context(etc::size_type const width,
		                        etc::size_type const height,
		                        Flags const flags,
		                        gl::renderer::Name const name);

	public:
		virtual ~Window();

		virtual
		etc::size_type poll();

		virtual
		etc::size_type poll(etc::size_type const max) = 0;

		virtual
		void confine_mouse(bool mode) = 0;

		virtual
		void swap_buffers() = 0;

	};

	struct RendererContext
	{
		virtual
		~RendererContext();
	};

	inline
	Window::Flags operator |(Window::Flags const lhs,
	                         Window::Flags const rhs) noexcept
	{ return (Window::Flags) (((int) lhs) | ((int) rhs)); }

	inline
	bool operator &(Window::Flags const lhs, Window::Flags const rhs) noexcept
	{ return (bool) (((int) lhs) & ((int) rhs)); }

}}} // !cube::system::window

#endif
