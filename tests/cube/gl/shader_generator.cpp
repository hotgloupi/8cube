#include <cube/gl/renderer.hpp>
#include <cube/gl/renderer/ShaderGenerator.hpp>
#include <cube/gl/viewport.hpp>
#include <cube/main_proto.hpp>
#include <cube/system/window.hpp>
#include <cube/system/inputs.hpp>

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
	auto vs_gen = window->renderer().generate_shader(ShaderType::vertex);
	auto fs_gen = window->renderer().generate_shader(ShaderType::fragment);
	vs_gen
		.parameter(ShaderParameterType::mat4, "cube_ModelViewPosition")
		.input(ShaderParameterType::vec3, "cube_Vertex")
		.output(ShaderParameterType::vec3, "cube_Position")
	;
	fs_gen
		.input(ShaderParameterType::vec3, "cube_Position")
		.output(ShaderParameterType::vec3, "cube_FragColor")
	;

	etc::print("----------- Generated vertex shader:");
	etc::print(vs_gen.source());
	etc::print("----------- Generated fragment shader:");
	etc::print(fs_gen.source());
	etc::print("-----------");
	auto vs = vs_gen.shader();
	auto fs = fs_gen.shader();

	bool running = true;
	auto slot = window->inputs().on_quit().connect(
		[&running]() { etc::print("Stop app"); running = false; }
	);
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
