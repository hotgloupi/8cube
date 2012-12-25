#include "ShaderProgram.hpp"
#include "Shader.hpp"

#include "_opengl.hpp"

#include "../Texture.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace cube { namespace gl { namespace renderer { namespace opengl {

	ETC_LOG_COMPONENT("cube.gl.renderer.opengl.ShaderProgram");

	ShaderProgram::ShaderProgram(std::vector<ShaderPtr>&& shaders)
		: _id{0}
	{
		_id = gl::CreateProgram();


		ETC_LOG.debug("Attaching shaders");
		for (auto& shader: shaders)
		{
			Shader const* opengl_shader = static_cast<Shader const*>(shader.get());
			assert(
				dynamic_cast<Shader const*>(shader.get()) != nullptr && "Wrong cast !?"
			);
			gl::AttachShader(_id, opengl_shader->id());
		}

		ETC_LOG.debug("Linking the program");
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

		ETC_LOG.debug("Validating the program");
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

	ShaderProgram::~ShaderProgram()
	{
		gl::DeleteProgram(_id);
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
		return ParameterPtr{new ShaderProgramParameter<Guard>{*this, _id, name}};
	}

}}}}
