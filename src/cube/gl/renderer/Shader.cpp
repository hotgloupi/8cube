#include "Shader.hpp"

namespace cube { namespace gl { namespace renderer {

	Shader::Shader(ShaderType const type)
		: type{type}
	{}

	Shader::Shader(ShaderType const type,
	               Parameters inputs,
	               Parameters outputs,
	               Parameters parameters) ETC_NOEXCEPT
		: type{type}
		, _inputs{std::move(inputs)}
		, _outputs{std::move(outputs)}
		, _parameters{std::move(parameters)}
	{}

	Shader::~Shader()
	{}

}}}
