#include "material.hpp"

#include <cube/gl/renderer/Renderer.hpp>
#include <cube/gl/renderer/Bindable.hpp>
#include <cube/gl/renderer/ShaderGenerator.hpp>
#include <cube/gl/renderer/ShaderProgram.hpp>
#include <cube/gl/renderer/Texture.hpp>
#include <cube/gl/renderer/Light.hpp>
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

	namespace {

		struct Bindable
			: public renderer::Bindable
		{
			ETC_LOG_COMPONENT("cube.gl.material.Bindable");

			MaterialPtr _material;
			renderer::ShaderProgramPtr _shader_program;
			std::vector<Guard> _guards;

			Bindable(Material& material,
			         renderer::Renderer& renderer,
			         renderer::ShaderProgramPtr shader_program)
				: _material(material.ptr())
				, _shader_program{std::move(shader_program)}
				, _guards{}
			{
				ETC_TRACE_CTOR();
				_guards.reserve(1 + _material->textures().size());
				for (Material::TextureChannel& ch: _material->textures())
				{
					if (ch.texture == nullptr)
					{
						if (ch.path.empty())
							throw Exception{"Empty texture path"};
						auto s = renderer.resource_manager().load<surface::Surface>(ch.path);
						ch.texture = renderer.new_texture(*s);
					}
				}
			}
			~Bindable()
			{ ETC_TRACE_DTOR(); }

			void _bind() override
			{
				ETC_TRACE.debug("Binding", *_material);
				auto& shader = *_shader_program;
				_guards.emplace_back(shader, this->shared_state());
				shader["cube_MVP"] = this->bound_state().mvp();
				shader["cube_Ambient"] = _material->ambient();

				if (_material->shading_model() != ShadingModel::none)
				{
					shader["cube_Diffuse"] = _material->diffuse();
					shader["cube_Specular"] = _material->specular();
					shader["cube_Shininess"] = _material->shininess();
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
							ETC_TRACE.debug("Update point light", point_light_idx, "with",
							                "position =", info.position,
							                "diffuse = ", info.diffuse,
							                "specular = ", info.specular);
							shader["cube_PointLightPosition"][point_light_idx] = info.position;
							shader["cube_PointLightDiffuse"][point_light_idx] =
								info.diffuse;
							shader["cube_PointLightSpecular"][point_light_idx] =
								info.specular;
							point_light_idx += 1;
						}
						else
							throw Exception{"Light kind '" + etc::to_string(light.kind) + "' not implemented"};
					}
					shader["cube_PointLightCount"] = point_light_idx;
				}

				int32_t idx = 0;
				for (auto& ch: _material->textures())
				{
					_guards.emplace_back(*ch.texture, this->shared_state());
					shader["cube_Texture" + std::to_string(idx)] = *ch.texture;
					idx += 1;
				}
			}

			void _unbind() ETC_NOEXCEPT override
			{
				_guards.clear();
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

					idx = 0;
					for (auto const& ch: _material.textures())
					{
						(void) ch;
						auto i = std::to_string(idx);
						res += "\tcube_TexCoord" + i + " = cube_VertexTexCoord" + i + ";\n";
					}

					if (_material.shading_model() != ShadingModel::none)
					{
						res +=
						"\tvec3 EyeNorm = "
							"normalize(cube_Normal * cube_VertexNormal);\n"
						"\tvec4 Eye = cube_ModelView * vec4(cube_Vertex, 1);\n"
						"\tfor (int i = 0; i < cube_PointLightCount; i++)\n"
						"\t{\n"
						"\t\tvec3 s = normalize(cube_PointLightPosition[i].xyz - Eye.xyz);\n"
						"\t\tvec3 v = normalize(-Eye.xyz);\n"
						"\t\tvec3 r = reflect(-s, EyeNorm);\n"
						"\t\tColor += vec4(\n"
							"\t\t\tcube_Diffuse * cube_PointLightDiffuse[i] * max(dot(s,EyeNorm), 0) + \n"
							"\t\t\tcube_Specular * cube_PointLightSpecular[i] * pow(max(dot(r, v), 0), cube_Shininess)\n"
							"\t\t\t, 1);\n"
						"\t}\n";
					}
					res += "\tcube_Color = clamp(Color, 0, 1);\n";
				}
				else if (proxy.type == renderer::ShaderType::fragment)
				{
					res += "\tvec4 Color = cube_Color;\n";
					int idx = 0;
					for (auto const& ch: _material.textures())
					{
						auto i = std::to_string(idx);
						auto tex = "cube_Texture" + i;
						auto coord = "cube_TexCoord" + i;
						if (ch.type == TextureType::ambient)
						{
							res += "\tColor += texture2D(" + tex
								+ ", " + coord + ".xy);\n";
						}
						else if (ch.type == TextureType::opacity)
						{
							res += "\tColor.a = texture2D(" + tex
								+ ", " + coord + ".xy).r;\n";
						}
						else
						{
							throw Exception{"Unimplemented texture type"};
						}
					}
					res += "\tcube_FragColor = Color;\n";
				}

				return res;
			}
		};

	} // !anonymous

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

		idx = 0;
		for (auto const& channel: _textures)
		{
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
