
#include "renderer.hpp"

# include <wrappers/boost/python.hpp>


BOOST_PYTHON_MODULE(renderer)
{
  cube::gl::renderer::detail::pyexport();
}
