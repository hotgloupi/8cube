#ifndef  CUBE_SYSTEM_WINDOW_HPP
# define CUBE_SYSTEM_WINDOW_HPP

# include <string>

# include <cube/gl/fwd.hpp>
# include <cube/gl/renderer/fwd.hpp>
# include <cube/system/fwd.hpp>

namespace cube { namespace system { namespace window {

	class Window
	{
	private:
		struct Impl;
		Impl* _impl;

	private:
		std::string _title;
		uint32_t _width;
		uint32_t _height;

	public:
		Window(std::string const& title, uint32_t width, uint32_t height);
		virtual ~Window();
		uint32_t poll();
		uint32_t poll(uint32_t max);
		::cube::gl::renderer::Renderer& renderer();
		::cube::system::inputs::Inputs& inputs();
		void confine_mouse(bool mode);
	};

}}} // !cube::system::window

#endif
