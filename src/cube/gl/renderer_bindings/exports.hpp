#ifndef  CUBE_GL_RENDERER_BINDINGS_EXPORTS_HPP
# define CUBE_GL_RENDERER_BINDINGS_EXPORTS_HPP

# include <wrappers/boost/python.hpp>

namespace cube { namespace gl { namespace renderer {}}}

namespace cube { namespace gl { namespace renderer_bindings {

	namespace py = boost::python;
	using namespace ::cube::gl::renderer;

	void export_constants();
	void export_bindable();
	void export_drawable();
	void export_state();
	void export_painter();
	void export_renderer();
	void export_vertex_buffer();
	void export_shader();
	void export_shader_program();
	void export_texture();

}}}

#endif

