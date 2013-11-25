#include "material.hpp"

#include <etc/test.hpp>

namespace cube { namespace gl { namespace material {

	Material::Material(std::string name)
		: _name{std::move(name)}
		, _shininess{0.0f}
		, _opacity{1.0f}
	{}

	namespace {

		ETC_TEST_CASE(material_default_values)
		{
			Material m;
			Material::color_type black;

			ETC_ENFORCE_EQ(m.name(), "unnamed");
			ETC_ENFORCE_EQ(m.diffuse(), black);
			ETC_ENFORCE_EQ(m.ambient(), black);
			ETC_ENFORCE_EQ(m.specular(), black);
			ETC_ENFORCE_EQ(m.shininess(), 0.0f);
			ETC_ENFORCE_EQ(m.opacity(), 1.0f);
			ETC_ENFORCE_EQ(m.textures().size(), 0lu);
		}

	} // !anonymous

}}} // !cube::gl::material
