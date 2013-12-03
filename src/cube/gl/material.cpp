#include "material.hpp"

#include <cube/gl/renderer/Renderer.hpp>
#include <cube/gl/renderer/Bindable.hpp>
#include <cube/gl/renderer/ShaderGenerator.hpp>
#include <cube/gl/renderer/ShaderProgram.hpp>
#include <cube/gl/renderer/Light.hpp>
#include <cube/gl/renderer.hpp>
#include <cube/system/window.hpp>

#include <etc/stack_ptr.hpp>
#include <etc/test.hpp>

namespace cube { namespace gl { namespace material {

	using cube::gl::exception::Exception;

	Material::Material(std::string name)
		: _name{std::move(name)}
		, _shininess{0.0f}
		, _opacity{1.0f}
		, _shading_model{ShadingModel::none}
	{}

	namespace {

		struct Bindable
			: public renderer::Bindable
		{
			Material const& _material;
			renderer::ShaderProgramPtr _shader_program;
			etc::stack_ptr<Guard>  _bind_guard;

			Bindable(Material const& material,
			         renderer::ShaderProgramPtr shader_program)
				: _material(material)
				, _shader_program{std::move(shader_program)}
				, _bind_guard{etc::stack_ptr_no_init}
			{}

			void _bind() override
			{
				auto& shader = *_shader_program;
				_bind_guard.reset(shader, this->bound_state());
				shader["cube_MVP"] = this->bound_state().mvp();
				shader["cube_Ambient"] = _material.ambient();

				if (_material.shading_model() == ShadingModel::none)
					return;
				shader["cube_Diffuse"] = _material.diffuse();
				shader["cube_Specular"] = _material.specular();
				shader["cube_Shininess"] = _material.shininess();
				int32_t point_light_idx = 0,
				        spot_light_idx = 0,
				        dir_light_idx = 0;
				shader["cube_ModelView"] = this->bound_state().model_view();
				shader["cube_Normal"] = this->bound_state().normal();
				for (auto const& ref: this->bound_state().lights())
				{
					auto const& light = ref.get();
					if (light.kind == renderer::LightKind::point)
					{
						auto const& info = light.point();
						shader["cube_PointLightPosition"][point_light_idx] = info.position;
						shader["cube_PointLightAmbient"][point_light_idx] =
							info.ambient;
						point_light_idx += 1;
					}
				}
				shader["cube_PointLightCount"] = point_light_idx;
			}

			void _unbind() noexcept override
			{
				_bind_guard.clear();
			}
		};

		struct Main
			: public renderer::ShaderRoutine
		{
			Material const& _material;
			Main(renderer::ShaderGeneratorProxy& proxy,
			     std::string name,
			     Material const& material)
				: renderer::ShaderRoutine{proxy, std::move(name)}
				, _material(material)
			{}

			std::string
			glsl_source() const override
			{
				std::string res;
				if (proxy.type == renderer::ShaderType::vertex)
				{
					res +=
						"\tgl_Position = cube_MVP * vec4(cube_Vertex, 1);\n"
						"\tvec4 Color = vec4(cube_Ambient, 1);\n"
					;

					int idx = 0;
					for (auto const& ch: _material.colors())
					{
						std::string in = "cube_VertexColor" + std::to_string(idx);
						if (ch.type == renderer::ShaderParameterType::vec3)
							res += "\tColor += vec4(" + in + ", 1);\n";
						else if (ch.type == renderer::ShaderParameterType::vec4)
							res += "\tColor += " + in + ";\n";
						else
							throw Exception{
								"Unhandled type " + etc::to_string(ch.type) +
								" for color channel " + std::to_string(idx)
							};
					}

					if (_material.shading_model() != ShadingModel::none)
					{
						res +=
						"\tvec3 EyeNorm = "
							"normalize(cube_Normal * cube_VertexNormal);\n"
						"\tvec4 Eye = cube_ModelView * vec4(cube_Vertex, 1);\n"
						"\tfor (int i = 0; i < cube_PointLightCount; i++)\n"
						"\t{\n\n\n"
						"\t\tvec3 s = normalize(cube_PointLightPosition[i].xyz - Eye.xyz);\n\n"
						"\t\tvec3 v = normalize(-Eye.xyz);\n"
						"\t\tvec3 r = reflect(-s, EyeNorm);\n"
						"\t\tColor += vec4(cube_PointLightAmbient[i] * (\n"
							"\t\t\tcube_Ambient + "
							"cube_Diffuse * max(dot(s,EyeNorm), 0) + \n"
							"cube_Specular * pow(max(dot(r, v), 0), cube_Shininess)\n"
							"), 1);\n"
						"\t}\n";
					}
					res += "\tcube_Color = Color;\n";
				}
				else if (proxy.type == renderer::ShaderType::fragment)
				{
					res += "\tcube_FragColor = cube_Color;\n";
				}

				return res;
			}
		};

	} // !anonymous

	renderer::BindablePtr
	Material::bindable(renderer::Renderer& renderer) const
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
			.parameter(8, ShaderParameterType::vec3, "cube_PointLightAmbient")
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
		).routine<Main>("main", *this);

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
			.routine<Main>("main", *this)
		;

		etc::print("vertex shader:", vs.source());
		etc::print("fragment shader:", fs.source());

		auto shader_program = renderer.new_shader_program(
			vs.shader(),
			fs.shader()
		);
		return renderer::BindablePtr{
			new Bindable{*this, std::move(shader_program)}
		};
	}

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
