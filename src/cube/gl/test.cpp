#include <etc/log.hpp>
#include <etc/print.hpp>

#include <cube/system/window.hpp>
#include <cube/system/inputs.hpp>

#include "opengl/test.hpp"

#include "renderer.hpp"
#include "renderer/VertexBuffer.hpp"
#include "renderer/Shader.hpp"
#include "renderer/ShaderProgram.hpp"
#include "vector.hpp"
#include "color.hpp"

#include "test.hpp"


namespace cube { namespace gl { namespace test {


	namespace {

		struct OnExpose
		{
			renderer::Renderer& renderer;
			void operator ()(float w, float h)
			{
				this->renderer.viewport(viewport::Viewport{0, 0, w, h});
			}
		};

		struct OnQuit
		{
			bool& running;
			void operator ()()
			{
				this->running = false;
			}
		};

	} // !anonymous

	void test_normal(system::window::Window& window,
	                 bool const& running)
	{
		auto vb = window.renderer().new_vertex_buffer();
		auto ib = window.renderer().new_index_buffer();
		auto sp = window.renderer().new_shader_program();

		vector::Vector2f vertices[] = {
			{.1, .1},
			{.9, .1},
			{.9, .9},
			{.1, .9},
		};
		vb->push_static_content(
			renderer::ContentKind::vertex,
			vertices,
			4
		);

		color::Color3f colors[] = {
			{"red"}, {"green"}, {"gray"}, {"blue"},
		};
		vb->push_static_content(
			renderer::ContentKind::color,
			colors,
			4
		);

		unsigned int indices[] = {0,1,2,3};
		ib->push_static_content(
			renderer::ContentKind::index,
			indices,
			4
		);

		{
			auto fs = window.renderer().new_fragment_shader();
			fs->push_source(
				"void main(void) {\n"
				"   gl_FragColor = gl_Color;\n"
				"}\n"
			);
			sp->push_shader(std::move(fs));
		}
		{
			auto vs = window.renderer().new_vertex_shader();
			vs->push_source(
				"uniform mat4 cube_ModelViewProjectionMatrix;"
				"void main(void)\n"
				"{\n"
				"   gl_FrontColor = gl_Color;\n"
				"   gl_Position = cube_ModelViewProjectionMatrix * gl_Vertex;\n"
				"}\n"
			);
			sp->push_shader(std::move(vs));
		}
		sp->finalize();


		vb->finalize();
		ib->finalize();


		auto painter = window.renderer().begin(
			renderer::Renderer::Mode::_2d
		);
		painter.bind(*vb);
		painter.bind(*sp);
		auto const& mvp = window.renderer().current_state().mvp;
		sp->parameter("cube_ModelViewProjectionMatrix") = mvp;

		size_t frame = 0;
		while (running)
		{
			etc::print("Frame", ++frame);
			window.poll();
			window.renderer().clear(
				renderer::BufferBit::color |
				renderer::BufferBit::depth
				);

			{
				painter.draw_elements(renderer::DrawMode::quads, *ib, 0, 4);
			}
			window.renderer().swap_buffers();

			::usleep(300);
		}
	}


	void test_all()
	{
		etc::log::debug("Starting Graphic Library tests");

		cube::system::window::Window window{"test window", 640, 480};
		window.inputs().on_expose().connect(OnExpose{window.renderer()});
		bool running = true;
		window.inputs().on_quit().connect(OnQuit{running});
		//opengl::test::vertex_buffer(window, running);
		test_normal(window, running);
	}

}}} // !cube::gl::test
