#include "material.hpp"

#include <cube/gl/renderer/Renderer.hpp>
#include <cube/gl/renderer/Bindable.hpp>
#include <cube/gl/renderer/ShaderGenerator.hpp>
#include <cube/gl/renderer/ShaderProgram.hpp>
#include <cube/gl/renderer.hpp>
#include <cube/system/window.hpp>

#include <etc/stack_ptr.hpp>
#include <etc/test.hpp>

namespace cube { namespace gl { namespace material {

	Material::Material(std::string name)
		: _name{std::move(name)}
		, _shininess{0.0f}
		, _opacity{1.0f}
	{}

	namespace {

		struct Bindable
			: public renderer::Bindable
		{
			Material const&        material;
			renderer::ShaderProgramPtr shader_program;
			etc::stack_ptr<Guard>  bind_guard;

			Bindable(Material const& material,
			         renderer::ShaderProgramPtr shader_program)
				: material(material)
				, shader_program{std::move(shader_program)}
				, bind_guard{etc::stack_ptr_no_init}
			{}

			void _bind(renderer::State const& s) override
			{
				bind_guard.reset(*shader_program, s);
				shader_program->parameter("cube_MVP") = s.mvp();
			}

			void _unbind() noexcept override
			{
				bind_guard.clear();
			}
		};

		struct Main
			: public renderer::ShaderRoutine
		{
			using ShaderRoutine::ShaderRoutine;
			std::string
			glsl_source() const override
			{
				std::string res;
				if (proxy.type == renderer::ShaderType::vertex)
				{
					res += "\tgl_Position = cube_MVP * vec4(cube_Vertex, 1);\n";
					res += "\tcube_Color = vec4(cube_VertexColor.xyz, 1);\n";
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
			.input(
				ShaderParameterType::vec3,
				"cube_Vertex",
				ContentKind::vertex
			)
			.input(
				ShaderParameterType::vec3,
				"cube_VertexColor",
				ContentKind::color
			)
			//.output(
			//	ShaderParameterType::vec3,
			//	"cube_Position",
			//	ContentKind::vertex
			//)
			.output(
				ShaderParameterType::vec4,
				"cube_Color",
				ContentKind::color
			)
			.routine<Main>("main")
		;

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
			.routine<Main>("main")
		;

		//etc::print("vertex shader:", vs.source());
		//etc::print("fragment shader:", fs.source());

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
