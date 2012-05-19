#ifndef  CUBE_SYSTEM_WINDOW_HPP
# define CUBE_SYSTEM_WINDOW_HPP

# include <string>

# include <boost/signal.hpp>

namespace cube { namespace system {

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
		cube::gl::Renderer& renderer();

	public:
		typedef boost::signal<void(uint32_t, uint32_t)> on_expose_t;
		typedef boost::signal<void()> on_idle_t;

	public:
		boost::signals::connection connect_expose(on_expose_t::slot_function_type subscribe_cb);
		boost::signals::connection connect_idle(on_idle_t::slot_function_type subscribe_cb);
	};

}} // !cube::system

#endif
