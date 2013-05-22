#include "ShaderGenerator.hpp"
#include "Renderer.hpp"

#include <etc/to_string.hpp>

namespace cube { namespace gl { namespace renderer { namespace opengl {

	namespace {

		struct GLSLProxy
			: public ShaderGenerator::Proxy
		{
			RendererType const& description;
			GLSLProxy(ShaderType const type,
			          ShaderGenerator& generator,
			          Renderer& renderer)
				: ShaderGenerator::Proxy{type, generator, renderer}
				, description(
					static_cast<RendererType const&>(
						renderer.description()
					)
				)
			{}

			std::string version_source() const
			{
				return etc::to_string(
					etc::iomanip::nosep(),
					"#version ",
					this->description.glsl.major,
					this->description.glsl.minor
				);
			}

			std::string parameter_source(Parameter const& p) const
			{
				std::string names[] = {
					"float_",
					"vec2",
					"vec3",
					"vec4",
					"int_",
					"ivec2",
					"ivec3",
					"ivec4",
					"bool_",
					"bvec2",
					"bvec3",
					"bvec4",
					"mat2",
					"mat3",
					"mat4",
					"sampler1d",
					"sampler2d",
					"sampler3d",
					"samplerCube",
					"sampler1DShadow",
					"sampler2DShadow",
				};

				return etc::to_string(
					"uniform", names[(int)p.first], p.second, ";"
				);
			}

			std::vector<std::string> sources() override
			{
				std::vector<std::string> res{};
				res.push_back(this->version_source());
				for (auto const& param: this->parameters)
					res.push_back(this->parameter_source(param));
				return res;
			}
		};

	} // !anonymous

	ShaderGenerator::ProxyPtr
	ShaderGenerator::begin(ShaderType const type)
	{
		return ProxyPtr{new GLSLProxy{type, *this, _renderer}};
	}

}}}}
