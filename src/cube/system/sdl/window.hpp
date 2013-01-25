#ifndef  CUBE_SYSTEM_SDL_WINDOW_HPP
# define CUBE_SYSTEM_SDL_WINDOW_HPP

# include "../window.hpp"

namespace cube { namespace system { namespace sdl { namespace window {

	class Window
		: public cube::system::window::Window
	{
	private:
		struct Impl;
		std::unique_ptr<Impl> _sdl_impl;

	public:
		Window(std::string const& title,
		       etc::size_type const width,
		       etc::size_type const height,
		       gl::renderer::Name const renderer_name);

		virtual
		~Window();

		etc::size_type poll(etc::size_type const max) override;
		void confine_mouse(bool mode) override;
		void swap_buffers() override;
	};

}}}}

#endif
