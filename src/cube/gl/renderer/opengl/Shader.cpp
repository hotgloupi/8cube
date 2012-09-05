
#include "Shader.hpp"

#include <etc/log.hpp>

namespace cube { namespace gl { namespace renderer { namespace opengl {

	ETC_LOG_COMPONENT("cube.gl.opengl.Shader");

	Shader::Shader(renderer::ShaderType const type)
		: type(type)
	{
		ETC_TRACE.debug("Create shader", this);
		_id = gl::CreateShader(gl::get_shader_type(type));
	}

	Shader::~Shader()
	{
		ETC_TRACE.debug("Delete shader", this);
		gl::DeleteShader(_id);
	}

	void Shader::_finalize(std::vector<std::string> const& sources)
	{
		ETC_TRACE.debug("Finalize shader with", sources.size(), "sources");
		if (sources.size() > 100)
			throw Exception{"Two much source in the shader."};
		char const* c_sources[100];
		for (size_t i = 0; i < sources.size(); ++i)
			c_sources[i] = sources[i].c_str();

		gl::ShaderSource(_id, sources.size(), c_sources, nullptr);


		gl::CompileShader(_id);
		GLint success = 0;
		gl::GetShaderiv(_id, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			GLchar log[4096];
			gl::GetShaderInfoLog(_id, sizeof(log), nullptr, log);
			throw Exception{
				"Cannot compile shader: " + std::string(log)
			};
		}
	}

}}}}
