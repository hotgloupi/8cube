#ifndef  CUBE_SYSTEM_SDL_WINDOW_HPP
# define CUBE_SYSTEM_SDL_WINDOW_HPP

# include "../window.hpp"

namespace cube { namespace system { namespace sdl { namespace window {

	class SDLRendererContext;

	class Window
		: public cube::system::window::Window
	{
	public:
		typedef cube::system::window::Window Super;

	public:
		Window(ImplPtr&& impl);

		virtual
		~Window();

		etc::size_type poll(etc::size_type const max) override;
		void confine_mouse(bool mode) override;
		void swap_buffers() override;

	public:
		static
		RendererContextPtr
		create_renderer_context(gl::renderer::Name const name);

	private:
		SDLRendererContext& _context();
	};

}}}}

#endif
