#include "ShaderGenerator.hpp"

#include "Renderer.hpp"

namespace cube { namespace gl { namespace renderer {

	ShaderGenerator::~ShaderGenerator()
	{}

	ShaderGenerator::Proxy::~Proxy()
	{}

	ShaderPtr
	ShaderGenerator::Proxy::shader()
	{
		return this->renderer.new_shader(this->type, this->sources());
	}

}}}
