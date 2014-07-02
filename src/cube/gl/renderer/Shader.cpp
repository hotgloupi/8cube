#include "Shader.hpp"

namespace cube { namespace gl { namespace renderer {

	Shader::Shader(ShaderType const type)
		: type{type}
	{}

	Shader::Shader(ShaderType const type,
	               Parameters inputs,
	               Parameters outputs,
	               Parameters parameters)
#if !(defined(__GNUC__) && __GNUC__ == 4 &&  __GNUC_MINOR__ < 8)
			ETC_NOEXCEPT_IF(std::is_nothrow_move_constructible<Parameters>())
#endif
		: type{type}
		, _inputs{std::move(inputs)}
		, _outputs{std::move(outputs)}
		, _parameters{std::move(parameters)}
	{}

	Shader::~Shader()
	{}

}}}
