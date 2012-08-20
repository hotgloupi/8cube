
#include "test.hpp"

#include <cube/gl/color.hpp>
#include <cube/gl/renderer.hpp>

#include "_opengl.hpp"

#include <unistd.h>

namespace cube { namespace gl { namespace opengl { namespace test {

    void vertex_buffer(system::window::Window& window,
                       bool const& running)
    {
      GLuint vertex_buffer, index_buffer;
      gl::GenBuffers(1, &vertex_buffer);
      gl::GenBuffers(1, &index_buffer);

      vector::Vector2f vertices[] = {
            {.1, .1},
            {.9, .1},
            {.9, .9},
            {.1, .9},
      };
      color::Color3f colors[] = {
            {"red"}, {"green"}, {"gray"}, {"blue"},
      };
      unsigned int indices[] = {0,1,2,3};
      etc::log::debug("vertices array size:", sizeof(vertices),
                      "colors array size:", sizeof(colors),
                      "indices array size:", sizeof(indices));
      gl::BindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
      gl::BufferData(GL_ARRAY_BUFFER,
                     sizeof(vertices) + sizeof(colors),
                     nullptr,
                     GL_STATIC_DRAW);
      gl::BufferSubData(GL_ARRAY_BUFFER,
                        0, sizeof(vertices),
                        vertices);
      gl::BufferSubData(GL_ARRAY_BUFFER,
                        sizeof(vertices), sizeof(colors),
                        colors);
      gl::BindBuffer(GL_ARRAY_BUFFER, 0);

      gl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
      gl::BufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
      //gl::BufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), nullptr, GL_STATIC_DRAW);
      //gl::BufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices);
      gl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

      while (running)
      {
        window.poll();
        window.renderer().clear(
            renderer::BufferBit::color |
            renderer::BufferBit::depth
        );

#define POINTER_OFFSET(offset) \
        static_cast<void*>(static_cast<char*>(nullptr) + offset) \
        /**/

        gl::BindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        gl::VertexPointer(vector::Vector2f::arity,
                          GL_FLOAT,
                          0,
                          POINTER_OFFSET(0));
        gl::ColorPointer(color::Color3f::arity,
                         GL_FLOAT,
                         0,
                         POINTER_OFFSET(sizeof(vertices)));

        gl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);


        gl::EnableClientState(GL_COLOR_ARRAY);
        gl::EnableClientState(GL_VERTEX_ARRAY);

        gl::DrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, nullptr);

        gl::DisableClientState(GL_VERTEX_ARRAY);
        gl::DisableClientState(GL_COLOR_ARRAY);

        gl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        gl::BindBuffer(GL_ARRAY_BUFFER, 0);

        window.renderer().swap_buffers();
        ::usleep(3);
      }
      gl::DeleteBuffers(1, &vertex_buffer);
      gl::DeleteBuffers(1, &index_buffer);
    }

}}}}

