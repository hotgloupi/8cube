#ifndef  CUBE_SYSTEM_WINDOW_HPP
# define CUBE_SYSTEM_WINDOW_HPP

# include <string>

# include <boost/signal.hpp>

namespace cube { namespace gl { namespace renderer {
    class Renderer;
}}}

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

	public:
# define EXPOSE_SIGNAL(name, ...)                                             \
		typedef boost::signal<void(__VA_ARGS__)> on_ ## name ## _t;           \
		boost::signals::connection                                            \
		connect_ ## name(on_ ## name ## _t::slot_function_type subscribe_cb)  \

		EXPOSE_SIGNAL(expose, uint32_t, uint32_t);
		EXPOSE_SIGNAL(idle, void);
		EXPOSE_SIGNAL(quit, void);

# undef EXPOSE_SIGNAL
	};

}}} // !cube::system::window

#endif
