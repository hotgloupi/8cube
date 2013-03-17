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
		ETC_TRACE.debug("Creating a new program");
		_id = gl::CreateProgram();

		ETC_LOG.debug("Attaching shaders to the program", _id);
		for (auto& shader: shaders)
		{
			Shader const* opengl_shader = static_cast<Shader const*>(shader.get());
			assert(
				dynamic_cast<Shader const*>(shader.get()) != nullptr && "Wrong cast !?"
			);
			gl::AttachShader(_id, opengl_shader->id());
		}

		ETC_LOG.debug("Linking the program", _id);
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

		ETC_LOG.debug("Validating the program", _id);
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
		ETC_TRACE.debug("Delete the program", _id);
		gl::DeleteProgram(_id);
	}

	void ShaderProgram::_bind()
	{
		ETC_TRACE.debug("Binding shader program", _id);
		gl::UseProgram(_id);
	}

	void ShaderProgram::_unbind()
	{
		ETC_TRACE.debug("Unbind current shader program", _id);
		gl::UseProgram(0);
	}

	template<typename BindGuard>
	class ShaderProgramParameter
		: public renderer::ShaderProgramParameter
	{
	private:
		GLint           _size;
		GLenum          _type;
		GLint           _location;

	public:
		// Program is bound by renderer::ShaderProgram::fetch_parameter()
		ShaderProgramParameter(ShaderProgram& program,
		                       std::string const& name,
		                       GLint size,
		                       GLenum type,
		                       GLint location)
			: renderer::ShaderProgramParameter{program, name}
			, _size{size}
			, _type{type}
			, _location{location}
		{}

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


	std::vector<ShaderProgram::ParameterPtr>
	ShaderProgram::_fetch_parameters()
	{
		GLint max_name_size;
		gl::GetProgramiv(_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_size);

		GLint nb_uniforms;
		gl::GetProgramiv(_id, GL_ACTIVE_UNIFORMS, &nb_uniforms);

		std::vector<ParameterPtr> res;
		if (max_name_size > 0 && nb_uniforms > 0)
		{
			std::vector<GLchar> name;
			name.resize(max_name_size);
			GLint size;
			GLint location;
			GLsizei written;
			GLenum type;
			for (GLint i = 0; i < nb_uniforms; ++i)
			{
				gl::GetActiveUniform(
					_id,            // program handle
					i,              // uniform index
					name.size(),    // buffer size
					&written,       // OUT: bytes written
					&size,          // OUT: size of the uniform
					&type,          // OUT: type of the uniform
					&name[0]        // OUT: name of the uniform
				);
				if (written <= 0)
				{
					ETC_LOG.warn("Cannot read uniform name at index", i);
					continue;
				}
				std::string valid_name{&name[0], static_cast<size_t>(written)};
				location = gl::GetUniformLocation(_id, valid_name.c_str());
				if (location < 0)
				{
					ETC_LOG.warn("Cannot find uniform location of", valid_name);
					continue;
				}
				res.emplace_back(
					new ShaderProgramParameter<Guard>{
						*this,
						valid_name,
						size,
						type,
						location,
					}
				);
			}
		}
		return res;
	}

}}}}
