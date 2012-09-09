#include "ShaderProgram.hpp"
#include "Shader.hpp"

#include "_opengl.hpp"

#include "../Texture.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace cube { namespace gl { namespace renderer { namespace opengl {

	ETC_LOG_COMPONENT("cube.gl.renderer.opengl.ShaderProgram");

	ShaderProgram::ShaderProgram()
		: _id{0}
		, _finalized{false}
	{
		_id = gl::CreateProgram();
	}

	ShaderProgram::~ShaderProgram()
	{
		gl::DeleteProgram(_id);
	}

	void ShaderProgram::_push_shader(renderer::Shader const& shader_)
	{
		Shader const* shader = static_cast<Shader const*>(&shader_);
		assert(
			dynamic_cast<Shader const*>(&shader_) == shader && "Wrong cast !?"
		);
		gl::AttachShader(_id, shader->id());
	}

	void ShaderProgram::_finalize()
	{
		ETC_TRACE.debug("Finalize the program");
		gl::LinkProgram(_id);
		GLint success = 0;
		glGetProgramiv(_id, GL_LINK_STATUS, &success);
		if (!success)
		{
			GLchar log[4096];
			gl::GetProgramInfoLog(_id, sizeof(log), nullptr, log);
			throw Exception{
				"Cannot link shader program: " + std::string(log)
			};
		}
		gl::ValidateProgram(_id);
		success = 0;
		glGetProgramiv(_id, GL_VALIDATE_STATUS, &success);
		if (!success)
		{
			GLchar log[4096];
			gl::GetProgramInfoLog(_id, sizeof(log), nullptr, log);
			throw Exception{
				"Cannot link shader program: " + std::string(log)
			};
		}
	}

	void ShaderProgram::_bind()
	{
		gl::UseProgram(_id);
	}

	void ShaderProgram::_unbind()
	{
		gl::UseProgram(0);
	}


	template<typename BindGuard>
	class ShaderProgramParameter
		: public renderer::ShaderProgramParameter
	{
	private:
		GLint           _location;
		std::string     _name;

	public:
		// Program is bound by renderer::ShaderProgram::fetch_parameter()
		ShaderProgramParameter(ShaderProgram& program,
		          GLuint id,
		          std::string const& name)
			: renderer::ShaderProgramParameter(program)
			, _location(0)
			, _name(name)
		{
			ETC_TRACE.debug("Retreive shader parameter", name);
			BindGuard guard(_program);
			_location = gl::GetUniformLocation(id, name.c_str());
			if (_location == -1)
				throw Exception{
					"Cannot find any active uniform variable called '" +
					name + "'."
				};
		}

		void operator =(matrix_type const& value)
		{
			ETC_TRACE.debug("Set shader parameter", _name, "to", value);
			BindGuard guard(_program);
			gl::UniformMatrix4fv(_location, 1, GL_FALSE,
			                     glm::value_ptr(value));
		}

		void operator =(int32_t value)
		{
			ETC_TRACE.debug("Set shader parameter", _name, "to", value);
			BindGuard guard(_program);
			gl::Uniform1i(_location, value);
		}
	};

	ShaderProgram::ParameterPtr
	ShaderProgram::_fetch_parameter(std::string const& name)
	{
		return ParameterPtr{new ShaderProgramParameter<BindGuard>{*this, _id, name}};
	}

}}}}
