
#include <cube/gl/renderer/Renderer.hpp>
#include <cube/main_proto.hpp>
#include <cube/system/window.hpp>
#include <cube/system/inputs.hpp>

#include <etc/path.hpp>
#include <etc/print.hpp>

#include <unistd.h>

using cube::system::window::create_window;

CUBE_MAIN_PROTO(int ac, char** av)
{
	etc::print("Dir:", etc::path::directory_name(av[0]));
	auto window = create_window("SimpleWindow", 640, 480);
	bool running = true;
	auto slot = window->inputs().on_quit().connect(
		[&running]() { etc::print("Stop app"); running = false; }
	);
	int i = 0;
	while (running)
	{
		window->renderer().initialize(window->renderer().viewport());
		window->renderer().clear();
		window->poll();
		window->swap_buffers();
		usleep(1000);
	}
	slot.disconnect();
	return 0;
}
