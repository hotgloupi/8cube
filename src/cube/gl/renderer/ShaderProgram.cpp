#include "ShaderProgram.hpp"
#include "Shader.hpp"

namespace cube { namespace gl { namespace renderer {

	void ShaderProgram::clear(bool parameters_too)
	{
		for (Shader* shader: _shaders)
			delete shader;
		_shaders.clear();

		if (parameters_too)
			_parameters.clear();
	}

}}}
