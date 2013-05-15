#include "ShaderGenerator.hpp"

namespace cube { namespace gl { namespace renderer { namespace opengl {

	namespace {

		struct GLSLProxy
			: public ShaderGenerator::Proxy
		{
			GLSLProxy(ShaderType const type,
			          ShaderGenerator& generator,
			          Renderer& renderer)
				: ShaderGenerator::Proxy{type, generator, renderer}
			{}

			std::vector<std::string> sources() override
			{
				return std::vector<std::string>{};
			}
		};

	} // !anonymous

	ShaderGenerator::ProxyPtr
	ShaderGenerator::begin(ShaderType const type)
	{
		return ProxyPtr{new GLSLProxy{type, *this, _renderer}};
	}

}}}}
