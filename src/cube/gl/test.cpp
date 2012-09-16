#include <etc/log.hpp>
#include <etc/print.hpp>

#include <cube/system/window.hpp>
#include <cube/system/inputs.hpp>

#include "renderer.hpp"
#include "renderer/VertexBuffer.hpp"
#include "renderer/Shader.hpp"
#include "renderer/ShaderProgram.hpp"
#include "renderer/Texture.hpp"

#include "color.hpp"
#include "font.hpp"
#include "vector.hpp"
#include "text.hpp"

#include "test.hpp"


namespace cube { namespace gl { namespace test {

	ETC_LOG_COMPONENT("cube.gl.test");


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
		renderer::Renderer::TexturePtr tex0;
		try {
			tex0 = std::move(window.renderer().new_texture("/home/hotgloupi/pif.png"));
		} catch(cube::exception::Exception const&) {
			tex0 = std::move(window.renderer().new_texture("/home/hotgloupi/Downloads/Water_snail_Rex_2.jpg"));
		}

		font::Font f(window.renderer(),
			//"/usr/share/fonts/truetype/ubuntu-font-family/Ubuntu-R.ttf",
			"/usr/share/fonts/truetype/droid/DroidSerif-Regular.ttf",
			48);
		text::Text text(f, u"Ceci est un test, et ça aussi !");

		vector::Vector2f vertices[] = {
			{10,     10},
			{630,    10},
			{630,    470},
			{10,     470},
		};
		vb->push_static_content(
			renderer::ContentKind::vertex,
			vertices,
			4
		);

		vector::Vector2f tex_coords[] = {
			{0,     0},
			{1,     0},
			{1,     1},
			{0,     1},
		};
		vb->push_static_content(
			renderer::ContentKind::tex_coord0,
			tex_coords,
			4
		);

		color::Color3f colors[] = {
			{"black"}, {"green"}, {"gray"}, {"blue"},
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
			auto vs = window.renderer().new_vertex_shader();
			vs->push_source(
				"uniform mat4 cube_ModelViewProjectionMatrix;\n"
				"void main(void)\n"
				"{\n"
				"   //gl_FrontColor = gl_Color;\n"
				"   gl_Position = cube_ModelViewProjectionMatrix * gl_Vertex;\n"
				"   gl_TexCoord[0] = gl_MultiTexCoord0;\n"
				"}\n"
			);
			sp->push_shader(std::move(vs));
		}
		{
			auto fs = window.renderer().new_fragment_shader();
			fs->push_source(
				"uniform sampler2D sampler0;"
				"void main(void) {\n"
				"   float r = texture2D(sampler0, vec2(gl_TexCoord[0])).r;\n"
				"   gl_FragColor = vec4(0,0,1,r);\n"
				"}\n"
			);
			sp->push_shader(std::move(fs));
		}

		sp->finalize();
		vb->finalize();
		ib->finalize();


		sp->parameter("cube_ModelViewProjectionMatrix");
		sp->parameter("sampler0") = *tex0;

		if (!tex0)
			throw "pif";

		while (running)
		{
			window.renderer().clear(
				renderer::BufferBit::color |
				renderer::BufferBit::depth
			);
			window.poll();
			if (auto painter = window.renderer().begin(renderer::Mode::_2d))
				{
					painter.bind(*sp);
					//painter.with(*vb, *tex0)->draw_elements(
					//	renderer::DrawMode::quads, *ib
					//);
					painter.draw(text, sp->parameter("sampler0"));

				}

			window.renderer().swap_buffers();

			::usleep(30);
		}
	}

	void test_texture(system::window::Window& window, bool& running)
	{
		auto sp = window.renderer().new_shader_program();
		{
			auto vs = window.renderer().new_vertex_shader();
			vs->push_source(
				"uniform mat4 cube_ModelViewProjectionMatrix;\n"
				"void main(void)\n"
				"{\n"
				"   //gl_FrontColor = gl_Color;\n"
				"   gl_Position = cube_ModelViewProjectionMatrix * gl_Vertex;\n"
				"   gl_TexCoord[0] = gl_MultiTexCoord0;\n"
				"}\n"
			);
			sp->push_shader(std::move(vs));
		}
		{
			auto fs = window.renderer().new_fragment_shader();
			fs->push_source(
				"uniform sampler2D sampler0;"
				"void main(void) {\n"
				"   float c = texture2D(sampler0, vec2(gl_TexCoord[0])).r;\n"
				"   gl_FragColor = vec4(c,c,c,1);\n"
				"}\n"
			);
			sp->push_shader(std::move(fs));
		}
		sp->finalize();
		sp->parameter("cube_ModelViewProjectionMatrix");

		auto tex = window.renderer().new_texture(
			renderer::PixelFormat::red,
			24,
			24,
			renderer::PixelFormat::red,
			renderer::ContentPacking::uint8,
			nullptr
		);

		{
			char buf[24*24];
			for (int x = 0; x < 24; ++x)
				for (int y = 0; y < 24; ++y)
				{
					buf[x + y * 24] = (x > 4 && x < 19 ? 255 : 64);
				}
			tex->set_data(0, 0, 24, 24,
			              renderer::PixelFormat::red,
			              renderer::ContentPacking::uint8,
			              buf);
		}

		sp->parameter("sampler0") = *tex;

		auto vb = window.renderer().new_vertex_buffer();
		{
			vector::Vector2f vertices[] = {
				{10,     10},
				{630,    10},
				{630,    470},
				{10,     470},
			};
			vb->push_static_content(
				renderer::ContentKind::vertex,
				vertices,
				4
			);

			vector::Vector2f tex_coords[] = {
				{0,     0},
				{1,     0},
				{1,     1},
				{0,     1},
			};
			vb->push_static_content(
				renderer::ContentKind::tex_coord0,
				tex_coords,
				4
			);
		}
		vb->finalize();

		auto ib = window.renderer().new_index_buffer();
		{
			unsigned int indices[] = {0,1,2,3};
			ib->push_static_content(
				renderer::ContentKind::index,
				indices,
				4
			);
		}
		ib->finalize();

		while (running)
		{
			window.renderer().clear(
				renderer::BufferBit::color |
				renderer::BufferBit::depth
			);
			window.poll();
			if (auto painter = window.renderer().begin(renderer::Mode::_2d))
				{
					painter.bind(*vb);
					painter.bind(*sp);
					painter.bind(*tex);
					painter.draw_elements(renderer::DrawMode::quads, *ib, 0, 4);
				}

			window.renderer().swap_buffers();

			::usleep(30);
		}
	}

	void test_all()
	{
		ETC_TRACE.debug("Starting Graphic Library tests");

		cube::system::window::Window window{"test window", 640, 480};
		window.inputs().on_expose().connect(OnExpose{window.renderer()});
		bool running = true;
		window.inputs().on_quit().connect(OnQuit{running});
		etc::print(window.renderer().description());
		test_normal(window, running);
	}

}}} // !cube::gl::test
