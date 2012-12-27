#include <wrappers/boost/python.hpp>

#include "../renderer/ShaderProgram.hpp"
#include "../renderer/Texture.hpp"

namespace cube { namespace gl { namespace renderer_bindings {

	namespace py = boost::python;
	using namespace ::cube::gl::renderer;

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
