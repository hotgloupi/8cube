#ifndef  CUBE_SYSTEM_WINDOW_HPP
# define CUBE_SYSTEM_WINDOW_HPP

# include <cube/gl/fwd.hpp>
# include <cube/gl/renderer/fwd.hpp>
# include <cube/system/fwd.hpp>

# include <string>
# include <memory>

namespace cube { namespace system { namespace window {

	class Window;

	std::unique_ptr<Window>
	create_window(std::string const& title,
	              etc::size_type const width,
	              etc::size_type const height,
	              gl::renderer::Name const name = gl::renderer::Name::OpenGL);

	class Window
	{
	private:
		struct Impl;
		std::unique_ptr<Impl> _impl;

	private:
		std::string     _title;
	protected:
		etc::size_type  _width;
		etc::size_type  _height;

	public:
		gl::renderer::Renderer& renderer();
		inputs::Inputs& inputs();

	public:
		Window(std::string const& title,
		       etc::size_type const width,
		       etc::size_type const height,
		       gl::renderer::Name const renderer_name);
		virtual ~Window();

		virtual
		etc::size_type poll();

		virtual
		etc::size_type poll(etc::size_type const max) = 0;

		virtual
		void confine_mouse(bool mode) = 0;

		virtual
		void swap_buffers() = 0;

	private:
		friend
		std::unique_ptr<Window>
		create_window(std::string const& title,
		              etc::size_type const width,
		              etc::size_type const height,
		              gl::renderer::Name const name);
	};

}}} // !cube::system::window

#endif
