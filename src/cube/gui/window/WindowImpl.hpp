#ifndef  CUBE_GUI_WINDOW_WINDOWIMPL_HPP
# define CUBE_GUI_WINDOW_WINDOWIMPL_HPP

# include "Window.hpp"
# include "RocketRenderInterface.hpp"

# include <cube/exception.hpp>
# include <cube/system/inputs.hpp>
# include <cube/system/window.hpp>

# include <etc/log.hpp>
# include <etc/stack_ptr.hpp>

namespace cube { namespace gui { namespace window {

	using cube::exception::Exception;
	ETC_LOG_COMPONENT("cube.gui.Window");

	struct Window::Impl
		: public Rocket::Core::EventListener
	{
		typedef Rocket::Core::Context context_t;
		typedef std::unique_ptr<context_t, void(*)(context_t*)> context_ptr_t;

		std::unique_ptr<cube::system::window::Window> root_window;
		std::unique_ptr<RocketRenderInterface> render_interface;
		context_ptr_t rocket_context;
		etc::stack_ptr<gl::vector::vec2i> new_size;
		std::vector<boost::signals2::connection> connections;

		Impl(std::unique_ptr<system::window::Window> root_window_);

		~Impl();

		void on_resize(int w, int h);

		void on_expose(int w, int h);

		void on_key(bool down,
		            system::inputs::KeyMod mod,
		            system::inputs::KeySym sym,
		            uint16_t);
		void on_mouse(bool down, uint8_t button, system::inputs::KeyMod mod);
		void on_mousemove(int32_t, int32_t, system::inputs::KeyMod mod);
	private:
		static void _delete_rocket_context(Rocket::Core::Context* ctx)
		{ ctx->RemoveReference(); }

		static
		Rocket::Core::Input::KeyIdentifier
		_to_rocket_sym(system::inputs::KeySym sym);

		static
		int _to_rocket_mod(system::inputs::KeyMod mod);

	private:
		// Rocket::Core::EventListener interface
		void ProcessEvent(Rocket::Core::Event& event) override;
	};

}}}

#endif
