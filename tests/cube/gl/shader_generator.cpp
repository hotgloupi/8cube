#include <cube/gl/renderer.hpp>
#include <cube/gl/viewport.hpp>
#include <cube/main_proto.hpp>
#include <cube/system/window.hpp>

#include <etc/path.hpp>
#include <etc/print.hpp>

#include <unistd.h>

using cube::system::window::create_window;
using namespace cube::gl::renderer;
using namespace cube::gl::viewport;

CUBE_MAIN_PROTO(int, char** av)
{
	etc::print("Dir:", etc::path::directory_name(av[0]));
	auto window = create_window("SimpleWindow", 640, 480);
	auto sources = window->renderer().generate_shader(ShaderType::vertex)
		->sources();

	int i = 0;
	for (auto const& line: sources)
		etc::print(++i, line);
//	bool running = true;
//	auto slot = window->inputs().on_quit().connect(
//		[&running]() { etc::print("Stop app"); running = false; }
//	);
//	while (running)
//	{
//		window->renderer().initialize(window->renderer().viewport());
//		window->renderer().clear();
//		window->poll();
//		window->swap_buffers();
//		usleep(1000);
//	}
//	slot.disconnect();
	return 0;
}
