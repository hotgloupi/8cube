
#include <cube/system/window.hpp>
#include <cube/system/inputs.hpp>
#include <cube/main_proto.hpp>
#include <unistd.h>

using cube::system::window::Window;

CUBE_MAIN_PROTO(int ac, char** av)
{
	Window window{"SimpleWindow", 640, 480};
	bool running = true;
	window.inputs().on_quit().connect(
		[&running]() { running = false; }
	);
	while (running)
	{
		window.poll();
		usleep(1000);
	}
}
