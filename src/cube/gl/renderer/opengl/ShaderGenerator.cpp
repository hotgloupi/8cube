#include "ShaderGenerator.hpp"
#include "Renderer.hpp"

#include <etc/to_string.hpp>

#include <sstream>

namespace cube { namespace gl { namespace renderer { namespace opengl {

	std::string ShaderGenerator::version_source() const
	{
		auto description = static_cast<RendererType const&>(
			this->_renderer.description()
		);
		return etc::to_string(
			etc::iomanip::nosep(),
			"#version ",
			description.glsl.major,
			description.glsl.minor
		);
	}

	std::string
	ShaderGenerator::parameter_type_source(ShaderParameterType const type) const
	{
		static std::string names[] = {
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
		return names[static_cast<int>(type)];
	}

	std::string ShaderGenerator::parameter_source(Parameter const& p) const
	{
		return etc::to_string(
			"uniform", parameter_type_source(p.first), p.second + ";"
		);
	}

	std::string ShaderGenerator::input_source(Parameter const& p) const
	{
		return etc::to_string(
			"in", parameter_type_source(p.first), p.second + ";"
		);
	}

	std::string ShaderGenerator::output_source(Parameter const& p) const
	{
		return etc::to_string(
			"out", parameter_type_source(p.first), p.second + ";"
		);
	}

	std::string ShaderGenerator::source(Proxy const& proxy) const
	{
#define CR "\n"
		std::stringstream ss;
		ss << this->version_source() << CR CR;

		ss << "// Parameters" << CR;
		for (auto const& param: proxy.parameters)
			ss << this->parameter_source(param) << CR;
		ss << CR;

		ss << "// Inputs" << CR;
		for (auto const& param: proxy.inputs)
			ss << this->input_source(param) << CR;
		ss << CR;


		ss << "// Outputs" << CR;
		for (auto const& param: proxy.outputs)
			ss << this->output_source(param) << CR;
		ss << CR;

		return ss.str();
#undef CR
	}


}}}}
