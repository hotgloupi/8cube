#ifndef  CUBE_GL_RENDERER_BINDINGS_EXPORTS_HPP
# define CUBE_GL_RENDERER_BINDINGS_EXPORTS_HPP

# include <cube/api.hpp>

namespace cube { namespace gl { namespace renderer_bindings {

	CUBE_API void export_bindable();
	CUBE_API void export_constants();
	CUBE_API void export_drawable();
	CUBE_API void export_painter();
	CUBE_API void export_renderer();
	CUBE_API void export_shader();
	CUBE_API void export_shader_generator();
	CUBE_API void export_shader_program();
	CUBE_API void export_state();
	CUBE_API void export_texture();
	CUBE_API void export_vertex_buffer();
	CUBE_API void export_vertex_buffer_attribute();

}}}

#endif

