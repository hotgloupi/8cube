
#include <cube/system/window.hpp>
#include <cube/system/inputs.hpp>
#include <cube/main_proto.hpp>

#include <etc/path.hpp>
#include <etc/print.hpp>

#include <unistd.h>

using cube::system::window::create_window;

CUBE_MAIN_PROTO(int ac, char** av)
{
	etc::print("Dir:", etc::path::directory_name(av[0]));
	auto window = create_window("SimpleWindow", 640, 480);
	bool running = true;
	window->inputs().on_quit().connect(
		[&running]() { running = false; }
	);
	int i = 0;
	while (i++ < 100)
	{
		window->poll();
		usleep(1000);
	}
	return 0;
}
