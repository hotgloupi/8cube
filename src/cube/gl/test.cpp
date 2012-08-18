#include <etc/log.hpp>

#include <cube/system/window.hpp>
#include <cube/system/inputs.hpp>

#include "opengl/test.hpp"

#include "renderer.hpp"
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

      vb->finalize();
      ib->finalize();

      while (running)
        {
          window.poll();
          window.renderer().clear(
              renderer::BufferBit::color |
              renderer::BufferBit::depth
          );

          {
            auto painter = window.renderer().begin(renderer::Renderer::Mode::_2d);
            painter.bind(*vb);
            painter.draw_elements(renderer::DrawMode::quads, *ib, 0, 4);
          }
          window.renderer().swap_buffers();

          ::usleep(300000);
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
}}}

