#include <cube/api.hpp>
#include <cube/gl/renderer/ShaderProgram.hpp>
#include <cube/gl/renderer/Texture.hpp>

#include <wrappers/boost/python.hpp>

namespace cube { namespace gl { namespace renderer_bindings {

	namespace py = boost::python;
	using namespace ::cube::gl::renderer;

	CUBE_API
	void export_texture()
	{
		py::class_<
				Texture,
				std::auto_ptr<Texture>,
				boost::noncopyable,
				py::bases<Bindable>
			>(
				"Texture",
				py::no_init
			)
			.def(
				"bind_unit",
				&Texture::bind_unit
			)
		;
	}

}}}
