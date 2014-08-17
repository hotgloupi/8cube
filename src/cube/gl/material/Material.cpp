#include "Material.hpp"
#include "ShaderRoutine.hpp"
#include "Bindable.hpp"

#include <cube/gl/renderer/Renderer.hpp>
#include <cube/gl/renderer/Texture.hpp>
#include <cube/gl/renderer.hpp>
#include <cube/gl/surface.hpp>
#include <cube/resource/Manager.hpp>
#include <cube/system/window.hpp>


#include <etc/test.hpp>
#include <etc/log.hpp>

namespace cube { namespace gl { namespace material {

	ETC_LOG_COMPONENT("cube.gl.material.Material");

	using cube::gl::exception::Exception;

	Material::Material(std::string name)
		: _name{std::move(name)}
		, _shininess{0.0f}
		, _opacity{1.0f}
		, _shading_model{ShadingModel::none}
	{ ETC_TRACE_CTOR(name); }

	renderer::BindablePtr
	Material::bindable(renderer::Renderer& renderer)
	{
		using renderer::ShaderType;
		using renderer::ShaderParameterType;
		using renderer::ContentKind;

		auto vs = renderer.generate_shader(ShaderType::vertex);
		vs.parameter(ShaderParameterType::float_, "cube_Shininess")
			.parameter(ShaderParameterType::float_, "cube_Opacity")
			.parameter(ShaderParameterType::vec3, "cube_Diffuse")
			.parameter(ShaderParameterType::vec3, "cube_Ambient")
			.parameter(ShaderParameterType::vec3, "cube_Specular")
			.parameter(ShaderParameterType::mat4, "cube_MVP")
			.parameter(ShaderParameterType::mat4, "cube_ModelView")
			.parameter(ShaderParameterType::mat3, "cube_Normal")
			.parameter(ShaderParameterType::int_, "cube_PointLightCount")
			.parameter(8, ShaderParameterType::vec3, "cube_PointLightPosition")
			.parameter(8, ShaderParameterType::vec3, "cube_PointLightDiffuse")
			.parameter(8, ShaderParameterType::vec3, "cube_PointLightSpecular")
			.input(
				ShaderParameterType::vec3,
				"cube_Vertex",
				ContentKind::vertex
			)
			.input(
				ShaderParameterType::vec3,
				"cube_VertexNormal",
				ContentKind::normal
			);
		int idx = 0;
		for (auto const& channel: _colors)
			vs.input(
				channel.type,
				"cube_VertexColor" + std::to_string(idx++),
				ContentKind::color
			);

		vs.output(
			ShaderParameterType::vec4,
			"cube_Color",
			ContentKind::color
		).routine<ShaderRoutine>("main", *this);

		auto fs = renderer.generate_shader(ShaderType::fragment);
		fs.input(
				ShaderParameterType::vec4,
				"cube_Color",
				ContentKind::color
			)
			.output(
				ShaderParameterType::vec4,
				"cube_FragColor",
				ContentKind::color
			)
			.routine<ShaderRoutine>("main", *this)
		;

		idx = 0;
		for (auto const& channel: _textures)
		{
			(void)channel;
			vs.input(
				ShaderParameterType::vec2,
				"cube_VertexTexCoord" + std::to_string(idx),
				ContentKind::tex_coord
			);
			vs.output(
				ShaderParameterType::vec2,
				"cube_TexCoord" + std::to_string(idx),
				ContentKind::tex_coord
			);
			fs.input(
				ShaderParameterType::vec2,
				"cube_TexCoord" + std::to_string(idx),
				ContentKind::tex_coord
			);
			fs.parameter(
				ShaderParameterType::sampler2d,
				"cube_Texture" + std::to_string(idx)
			);
			idx += 1;
		}
		//etc::print("vertex shader:", vs.source());
		//etc::print("fragment shader:", fs.source());

		auto shader_program = renderer.new_shader_program(
			vs.shader(),
			fs.shader()
		);
		return renderer::BindablePtr{
			new Bindable{*this, renderer, std::move(shader_program)}
		};
	}

	void Material::print(std::ostream& out) const ETC_NOEXCEPT
	{ out << "<Material " << this->_name << ">"; }

	namespace {

		ETC_TEST_CASE(material_default_values)
		{
			Material m;
			Material::color_type black;

			ETC_ENFORCE_EQ(m.name(), "unamed");
			ETC_ENFORCE_EQ(m.diffuse(), black);
			ETC_ENFORCE_EQ(m.ambient(), black);
			ETC_ENFORCE_EQ(m.specular(), black);
			ETC_ENFORCE_EQ(m.shininess(), 0.0f);
			ETC_ENFORCE_EQ(m.opacity(), 1.0f);
			ETC_ENFORCE_EQ(m.textures().size(), 0lu);
		}

	} // !anonymous

}}} // !cube::gl::material
