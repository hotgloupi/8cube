#include "ShaderProgram.hpp"
#include "Shader.hpp"

#include "_opengl.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace cube { namespace gl { namespace opengl {

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

	namespace { namespace implem {

		class Parameter
			: public ShaderProgram::Parameter
		{
		private:
			GLint _location;

		public:
			Parameter(GLuint program, std::string const& name)
			{
				_location = gl::GetUniformLocation(program, name.c_str());
				if (_location == -1)
					throw Exception{
						"Cannot find any active uniform variable called '" +
						name + "'."
					};
			}

			virtual
			ShaderProgram::Parameter&
			operator =(matrix_type const& value)
			{
				gl::UniformMatrix4fv(_location, 1, GL_FALSE, glm::value_ptr(value));
				return *this;
			}
		};

	}} // !anonymous::implem

	ShaderProgram::ParameterPtr
	ShaderProgram::_fetch_parameter(std::string const& name)
	{
		return ParameterPtr{new implem::Parameter{_id, name}};
	}

}}}
