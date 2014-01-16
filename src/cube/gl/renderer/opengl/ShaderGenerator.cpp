#include "Exception.hpp"
#include "Renderer.hpp"
#include "ShaderGenerator.hpp"

#include <etc/to_string.hpp>
#include <etc/enum.hpp>

#include <sstream>
#include <unordered_map>

namespace cube { namespace gl { namespace renderer { namespace opengl {

	struct ShaderGenerator::Impl
	{
		RendererType const& description;
		int glsl_version;
		std::string vertex_in_qualifier;
		std::string vertex_out_qualifier;
		std::string fragment_in_qualifier;
		std::string fragment_out_qualifier;

		Impl(RendererType const& description)
			: description(description)
			, glsl_version{
				this->description.glsl.major * 100 +
				this->description.glsl.minor
			}
		{
			if (this->glsl_version > 120) // XXX check version
			{
				this->vertex_in_qualifier = "in";
				this->vertex_out_qualifier = "out";
				this->fragment_in_qualifier = "in";
				this->fragment_out_qualifier = "out";
			}
			else
			{
				this->vertex_in_qualifier = "attribute";
				this->vertex_out_qualifier = "varying";
				this->fragment_in_qualifier = "varying";
				this->fragment_out_qualifier = "varying";
			}
		}

		std::string version_source() const
		{
			return etc::to_string(
				etc::io::nosep(), "#version ", this->glsl_version
			);
		}

		std::string
		parameter_type_source(ShaderParameterType const type) const
		{
			static std::string names[] = {
				"float",
				"vec2",
				"vec3",
				"vec4",
				"int",
				"ivec2",
				"ivec3",
				"ivec4",
				"bool",
				"bvec2",
				"bvec3",
				"bvec4",
				"mat2",
				"mat3",
				"mat4",
				"sampler1D",
				"sampler2D",
				"sampler3D",
				"samplerCube",
				"sampler1DShadow",
				"sampler2DShadow",
			};
			return names[static_cast<int>(type)];
		}

		std::string
		parameter_source(Proxy const&, Parameter const& p) const
		{
			return etc::to_string(
				"uniform",
				this->parameter_type_source(p.type),
				p.name + (
					p.array_size > 0 ?
					"[" + std::to_string(p.array_size) + "]" :
					""
				) + ";"
			);
		}

		typedef
			std::unordered_map<ContentKind, std::string, etc::enum_hash>
			builtin_map_type;

		static builtin_map_type vertex_attribute_builtins;
		static builtin_map_type vertex_varying_builtins;
		static builtin_map_type fragment_varying_builtins;

		std::string input_source(Proxy const& proxy, Parameter const& p) const
		{
			std::string result;
			if (this->glsl_version < 120)
			{
				builtin_map_type* map = nullptr;
				if (proxy.type == ShaderType::vertex)
					map = &vertex_attribute_builtins;
				else if (proxy.type == ShaderType::fragment)
					map = &fragment_varying_builtins;

				std::string builtin;
				if (map != nullptr)
				{
					auto it = map->find(p.content_kind);
					if (it != map->end())
						builtin = it->second;
				}

				if (!builtin.empty())
				{
					result += etc::to_string(
						"#define",
						p.name,
						builtin
					) + "\n";
					result += "// ";
				}
			}
			result += etc::to_string(
				(
					proxy.type == ShaderType::vertex ?
					this->vertex_in_qualifier :
					this->fragment_in_qualifier
				),
				this->parameter_type_source(p.type),
				p.name + ";"
			);
			return result;
		}

		std::string output_source(Proxy const& proxy, Parameter const& p) const
		{
			std::string result;
			if (proxy.type == ShaderType::vertex)
			{
				if (p.content_kind == ContentKind::vertex)
				{

					result += etc::to_string(
							"#define",
							p.name,
							"gl_Position"
						) + "\n//";
				}
				else if (this->glsl_version < 120)
				{
					auto it = vertex_varying_builtins.find(p.content_kind);
					if (it != vertex_varying_builtins.end())
						result += etc::to_string(
							"#define",
							p.name,
							it->second
						) + "\n//";
				}
			}
			else if (proxy.type == ShaderType::fragment)
			{
				if (p.content_kind == ContentKind::color && this->glsl_version <= 120)
				{
					result += etc::to_string(
						"#define",
						p.name,
						"gl_FragColor"
					) + "\n//";
				}

			}
			result += etc::to_string(
				(
					proxy.type == ShaderType::vertex ?
					this->vertex_out_qualifier :
					this->fragment_out_qualifier
				),
				this->parameter_type_source(p.type),
				p.name + ";"
			);
			return result;
		}

		std::string routine_source(Proxy const&,
		                           ShaderRoutine const& r) const
		{
			std::string result;
			if (r.return_type.is_void)
				result += "void";
			else
				result += this->parameter_type_source(r.return_type.type);
			result += " ";
			result += r.name;
			result += "(";
			bool first_arg = true;
			for (auto const& arg: r.in_arguments)
			{
				if (first_arg)
					first_arg = false;
				else
					result += ", ";
				result += "in ";
				result += this->parameter_type_source(arg.type);
				result += arg.name;
			}
			result += ")\n";
			result += "{\n";
			result += r.source(ShaderLanguage::glsl);
			if (result.back() != '\n')
				result += "\n";
			result += "}\n";
			return result;
		}
	};

	ShaderGenerator::Impl::builtin_map_type
	ShaderGenerator::Impl::vertex_attribute_builtins = {
		{ContentKind::color, "gl_Color"},
		{ContentKind::color1, "gl_SecondaryColor"},
		{ContentKind::normal, "gl_Normal"},
		{ContentKind::vertex, "gl_Vertex"},
		{ContentKind::tex_coord0, "gl_MultiTexCoord0"},
		{ContentKind::tex_coord1, "gl_MultiTexCoord1"},
		{ContentKind::tex_coord2, "gl_MultiTexCoord2"},
		{ContentKind::tex_coord3, "gl_MultiTexCoord3"},
		{ContentKind::tex_coord4, "gl_MultiTexCoord4"},
		{ContentKind::tex_coord5, "gl_MultiTexCoord5"},
		{ContentKind::tex_coord6, "gl_MultiTexCoord6"},
		{ContentKind::tex_coord7, "gl_MultiTexCoord7"},
	};

	ShaderGenerator::Impl::builtin_map_type
	ShaderGenerator::Impl::vertex_varying_builtins = {
		{ContentKind::color, "gl_FrontColor"},
		//{ContentKind::, "gl_BackColor"},
		{ContentKind::color1, "gl_FrontSecondaryColor"},
		//{"gl_BackSecondaryColor"},
		{ContentKind::tex_coord0, "gl_TexCoord[0]"},
		{ContentKind::tex_coord1, "gl_TexCoord[1]"},
		{ContentKind::tex_coord2, "gl_TexCoord[2]"},
		{ContentKind::tex_coord3, "gl_TexCoord[3]"},
		{ContentKind::tex_coord4, "gl_TexCoord[4]"},
		{ContentKind::tex_coord5, "gl_TexCoord[5]"},
		{ContentKind::tex_coord6, "gl_TexCoord[6]"},
		{ContentKind::tex_coord7, "gl_TexCoord[7]"},
	};
	ShaderGenerator::Impl::builtin_map_type
	ShaderGenerator::Impl::fragment_varying_builtins = {
		{ContentKind::color0, "gl_Color"},
		{ContentKind::color1, "gl_SecondaryColor"},
		{ContentKind::tex_coord0, "gl_TexCoord[0]"},
		{ContentKind::tex_coord1, "gl_TexCoord[1]"},
		{ContentKind::tex_coord2, "gl_TexCoord[2]"},
		{ContentKind::tex_coord3, "gl_TexCoord[3]"},
		{ContentKind::tex_coord4, "gl_TexCoord[4]"},
		{ContentKind::tex_coord5, "gl_TexCoord[5]"},
		{ContentKind::tex_coord6, "gl_TexCoord[6]"},
		{ContentKind::tex_coord7, "gl_TexCoord[7]"},
	};

	ShaderGenerator::ShaderGenerator(Renderer& renderer)
		: renderer::ShaderGenerator{renderer}
		, _this{new Impl{
			dynamic_cast<RendererType const&>(this->_renderer.description())
		}}
	{}

	ShaderGenerator::~ShaderGenerator()
	{}

	std::string ShaderGenerator::source(Proxy const& proxy) const
	{
		if (proxy.type == ShaderType::vertex)
		{
			auto it = std::find_if(
				proxy.inputs.begin(),
				proxy.inputs.end(),
				[] (Proxy::Parameter const& param) {
					return param.content_kind == ContentKind::vertex;
				}
			);
			if (it == proxy.inputs.end())
				throw Exception{
					"Vertex input attribute is required"
				};
		}
		else if (proxy.type == ShaderType::fragment)
		{

		}
		else
			throw Exception{
				"Unsupported shader type " + etc::to_string(proxy.type)
			};

#define CR "\n"
		std::stringstream ss;
		ss << _this->version_source() << CR CR;

		ss << "// Parameters" << CR;
		for (auto const& param: proxy.parameters)
			ss << _this->parameter_source(proxy, param) << CR;
		ss << CR;

		ss << "// Inputs" << CR;
		for (auto const& param: proxy.inputs)
			ss << _this->input_source(proxy, param) << CR;
		ss << CR;

		ss << "// Outputs" << CR;
		for (auto const& param: proxy.outputs)
			ss << _this->output_source(proxy, param) << CR;
		ss << CR;

		ss <<  "// Routines" << CR;
		for (auto const& routine: proxy.routines)
			ss << _this->routine_source(proxy, *routine) << CR;

		return ss.str();
#undef CR
	}

}}}}
