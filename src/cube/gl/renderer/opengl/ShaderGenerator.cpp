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
		std::string in_qualifier;
		std::string out_qualifier;

		Impl(RendererType const& description)
			: description(description)
			, glsl_version{
				this->description.glsl.major * 100 +
				this->description.glsl.minor
			}
		{
			if (this->glsl_version > 120) // XXX check version
			{
				this->in_qualifier = "in";
				this->out_qualifier = "out";
			}
			else
			{
				this->in_qualifier = "attribute";
				this->out_qualifier = "varying";
			}
		}

		std::string version_source() const
		{
			return etc::to_string(
				etc::iomanip::nosep(), "#version ", this->glsl_version
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
				"uniform", this->parameter_type_source(p.type), p.name + ";"
			);
		}

		std::string input_source(Proxy const& proxy, Parameter const& p) const
		{
			std::string result;
			if (this->glsl_version <= 120 and
				proxy.type != ShaderType::vertex)
			{
				static std::unordered_map<ContentKind, std::string, etc::enum_hash> builtin_names{
					{ContentKind::color, "gl_Color"},
					{ContentKind::vertex, "gl_VertexPosition"},
					{ContentKind::tex_coord0, "gl_TexCoord0"},
				};
				if (builtin_names.find(p.content_kind) != builtin_names.end())
					result += etc::to_string(
						"#define",
						p.name,
						"gl_Color"
					) + "\n";
				result += "// ";
			}
			result += etc::to_string(
				this->in_qualifier,
				this->parameter_type_source(p.type),
				p.name + ";"
			);
			return result;
		}

		std::string output_source(Proxy const&, Parameter const& p) const
		{
			std::string result;
			result += etc::to_string(
				this->out_qualifier,
				this->parameter_type_source(p.type),
				p.name + ";"
			);
			if (this->glsl_version <= 120)
			{
				if (p.content_kind == ContentKind::color)
				{
					return etc::to_string(
						"#define",
						p.name,
						"gl_FragColor"
					);
				}
			}
			return result;
		}
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

		return ss.str();
#undef CR
	}

}}}}
