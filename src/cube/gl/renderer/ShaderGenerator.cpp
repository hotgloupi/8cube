#include "ShaderGenerator.hpp"

#include "Renderer.hpp"

namespace cube { namespace gl { namespace renderer {

	ShaderGenerator::~ShaderGenerator()
	{}

	ShaderGeneratorProxy
	ShaderGenerator::begin(ShaderType const type)
	{
		return {type, *this, _renderer};
	}

	ShaderPtr
	ShaderGeneratorProxy::shader()
	{
		return this->renderer.new_shader(this->type, {this->source()});
	}

}}}
