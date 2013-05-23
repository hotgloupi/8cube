#include <cube/gl/renderer.hpp>
#include <cube/gl/renderer/ShaderGenerator.hpp>
#include <cube/gl/viewport.hpp>
#include <cube/main_proto.hpp>
#include <cube/system/window.hpp>

#include <etc/path.hpp>
#include <etc/print.hpp>

#include <unistd.h>

using cube::system::window::create_window;
using namespace cube::gl::renderer;
using namespace cube::gl::viewport;

CUBE_MAIN_PROTO(int, char**)
{
	//	Does not work, create_renderer requires a context
	// auto renderer = create_renderer(Viewport{0, 0, 640, 480});
	// auto sources = renderer->generate_shader(ShaderType::vertex)
	// 	->sources();

	auto window = create_window("SimpleWindow", 640, 480);
	auto generator = window->renderer().generate_shader(ShaderType::vertex);
	generator
		.parameter(ShaderParameterType::mat4, "cube_ModelViewPosition")
		.in(ShaderParameterType::vec3, "cube_Vertex")
		.out(ShaderParameterType::vec3, "cube_Position")
	;


	etc::print("----------- Generated shader:");
	etc::print(generator.source());
	etc::print("-----------");
	auto shader = generator.shader();

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
