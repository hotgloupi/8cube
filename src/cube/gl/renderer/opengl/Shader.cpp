
#include "Shader.hpp"

#include <etc/log.hpp>

namespace cube { namespace gl { namespace renderer { namespace opengl {

	ETC_LOG_COMPONENT("cube.gl.renderer.opengl.Shader");

	Shader::Shader(renderer::ShaderType const type,
	               std::vector<std::string> const& sources)
		: type{type}
	{
		_id = gl::CreateShader(gl::get_shader_type(type));
		ETC_TRACE_CTOR(type, "with", sources.size(), "sources");

		{
			// Adding sources to the shader
			std::vector<char const*> c_sources;
			for (auto const& source : sources)
				c_sources.push_back(source.c_str());

			gl::ShaderSource(_id, c_sources.size(), &c_sources[0], nullptr);
		}

		gl::CompileShader(_id);
		GLint success = 0;
		gl::GetShaderiv(_id, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			std::vector<GLchar> log{};
			log.resize(4096);
			gl::GetShaderInfoLog(_id, log.size(), nullptr, &log[0]);
			throw Exception{
				"Cannot compile shader: " + std::string(&log[0])
			};
		}
	}

	Shader::~Shader()
	{
		ETC_TRACE_DTOR(_id);
		gl::DeleteShader<gl::no_throw>(_id);
	}

}}}}
