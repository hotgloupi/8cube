#include "ShaderProgram.hpp"
#include "Shader.hpp"

#include "_opengl.hpp"

#include <cube/gl/renderer/Texture.hpp>
#include <cube/gl/color.hpp>

#include <etc/assert.hpp>
#include <etc/printable.hpp>

#include <glm/gtc/type_ptr.hpp>

namespace cube { namespace gl { namespace renderer { namespace opengl {

	ShaderProgram::ShaderProgram(std::vector<ShaderPtr>&& shaders)
		: _id{0}
	{
		_id = gl::CreateProgram();
		ETC_TRACE_CTOR(_id);

		ETC_LOG.debug(*this, "Attach shaders to the program", _id);
		for (auto& shader: shaders)
		{
			Shader const* opengl_shader =
				static_cast<Shader const*>(shader.get());
			assert(
				dynamic_cast<Shader const*>(shader.get()) != nullptr &&
				"Wrong cast !?"
			);
			gl::AttachShader(_id, opengl_shader->id());
		}

		std::string frag_data_var;
		for (auto& shader: shaders)
		{
			if (shader->type == ShaderType::vertex)
			{
				ETC_TRACE.debug(*this, "Bind attributes of", shader);
				for (auto const& input: shader->inputs())
				{
					ETC_LOG.debug(
						"Bind input", input.name.c_str(),
						"to index", input.content_kind,
						"(" + std::to_string((int) input.content_kind) + ")"
					);
					gl::BindAttribLocation(
						_id, (int) input.content_kind, input.name.c_str()
					);
				}
			}
			else if (shader->type == ShaderType::fragment)
			{
				for (auto const& output: shader->outputs())
				{
					if (output.content_kind == ContentKind::color)
					{
						ETC_ASSERT_EQ(frag_data_var, ""); // one frag data.
						ETC_LOG.debug("Bind fragment output to", output.name);
						gl::BindFragDataLocation(
							_id,
							0, //(int) output.content_kind,
							output.name.c_str()
						);
						frag_data_var = output.name;
					}
				}
			}
		}

		ETC_LOG.debug(*this, "Link the program", _id);
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

		ETC_LOG.debug(*this, "Validate the program", _id);
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
		GLint nb_attributes, max_name_size;
		GLint written, size, location;
		GLenum type;
		glGetProgramiv(_id, GL_ACTIVE_ATTRIBUTES, &nb_attributes);
		glGetProgramiv(_id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_name_size);
		std::vector<char> name(max_name_size);
		for (int i = 0; i < nb_attributes; ++i)
		{
			glGetActiveAttrib(_id, i, max_name_size,
			                  &written, &size, &type, &name[0]);
			location = glGetAttribLocation(_id, &name[0]);
			ETC_LOG.debug(
				"Active bound attribute:", &name[0], "at index", location
			);
		}

		if (!frag_data_var.empty())
			ETC_LOG.debug(
				"FragData named", frag_data_var, "at index ",
				glGetFragDataLocation(_id, frag_data_var.c_str())
			);

	}

	ShaderProgram::~ShaderProgram()
	{
		ETC_TRACE_DTOR(_id);
		gl::DeleteProgram<gl::no_throw>(_id);
	}

	void ShaderProgram::_bind()
	{
		ETC_TRACE.debug(*this, "Bind shader program", _id);
		gl::UseProgram(_id);
	}

	void ShaderProgram::_unbind() ETC_NOEXCEPT
	{
		Super::_unbind();
		ETC_TRACE.debug(*this, "Unbind current shader program", _id);
		gl::UseProgram<gl::no_throw>(0);
	}

	template<typename BindGuard>
	class ShaderProgramParameter
		: public renderer::ShaderProgramParameter
		, public etc::Printable
	{
		ETC_LOG_COMPONENT("cube.gl.renderer.opengl.ShaderProgramParameter");
	private:
		GLint           _size;
		GLenum          _type;
		GLint           _location;
		std::unordered_map<etc::size_type, std::unique_ptr<renderer::ShaderProgramParameter>> _indexes;

	public:
		// Program is bound by renderer::ShaderProgram::fetch_parameter()
		ShaderProgramParameter(renderer::ShaderProgram& program,
		                       std::string const& name,
		                       GLint size,
		                       GLenum type,
		                       GLint location)
			: renderer::ShaderProgramParameter{program, name}
			, _size{size}
			, _type{type}
			, _location{location}
		{}

		renderer::ShaderProgramParameter&
		_at(etc::size_type const idx) override
		{
			ETC_TRACE.debug(
				"Retreive parameter", _name + "[" + std::to_string(idx) + "]"
			);
			if (idx == 0)
				return *this;
			auto it = _indexes.find(idx);
			if (it != _indexes.end())
				return *it->second;
			ETC_LOG.debug("First time fetch, creating parameter");
			std::string name = this->name() + "[" + std::to_string(idx) + "]";
			GLint loc = -1;
			{
				BindGuard guard(_program);
				loc = gl::GetUniformLocation(
					static_cast<ShaderProgram&>(_program).opengl_id(),
					name.c_str()
				);
			}
			if (loc < 0)
				throw Exception{"Cannot find uniform location of" + name};
			ETC_LOG.debug("Found active uniform", name, "at", loc);
			_indexes[idx] = std::unique_ptr<renderer::ShaderProgramParameter>{
				new ShaderProgramParameter{
					_program,
					name,
					_size,
					_type,
					loc,
				}
			};
			return *_indexes[idx];
		}

		void _set(matrix::mat4f const& value) override
		{
			ETC_TRACE.debug(*this, "Set shader parameter", _name, "to", value);
			BindGuard guard(_program);
			gl::UniformMatrix4fv(_location, 1, GL_FALSE, glm::value_ptr(value));
		}

		void _set(matrix::mat3f const& value) override
		{
			ETC_TRACE.debug(*this, "Set shader parameter", _name, "to", value);
			BindGuard guard(_program);
			gl::UniformMatrix3fv(_location, 1, GL_FALSE, glm::value_ptr(value));
		}

		void _set(int32_t const value) override
		{
			ETC_TRACE.debug(*this, "Set shader parameter", _name, "to", value);
			BindGuard guard(_program);
			gl::Uniform1i(_location, value);
		}

		void _set(float const value) override
		{
			ETC_TRACE.debug(*this, "Set shader parameter", _name, "to", value);
			BindGuard guard(_program);
			gl::Uniform1f(_location, value);
		}

		void _set(vector::vec3f const& value) override
		{
			ETC_TRACE.debug(*this, "Set shader parameter", _name, "to", value);
			BindGuard guard(_program);
			gl::Uniform3fv(_location, 1, glm::value_ptr(value));
		}

		void _set(color::Color3f const& value) override
		{
			ETC_TRACE.debug(*this, "Set shader parameter", _name, "to", value, value.colors[0], value.colors[1], value.colors[2]);
			BindGuard guard(_program);
			gl::Uniform3fv(_location, 1, &value.colors[0]);
		}

		void print(std::ostream& out) const ETC_NOEXCEPT
		{
			out << "<OpenGLShaderProgramParameter '" << _name << "'>";
		}
	};


	std::vector<ShaderProgram::ParameterPtr>
	ShaderProgram::_fetch_parameters()
	{
		ETC_TRACE.debug(*this, "Fetching parameters");
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
				if (valid_name.find('[') != std::string::npos)
				{
					// Nvidia choose to expose names ending with '[0]' for
					// arrays, we ignore silently the fact  that we have an
					// array (as it's not portable).
					valid_name.resize(valid_name.find('['));
				}
				location = gl::GetUniformLocation(_id, valid_name.c_str());
				if (location < 0)
				{
					ETC_LOG.warn("Cannot find uniform location of", valid_name);
					continue;
				}
				ETC_LOG.debug(
					"Found active uniform", valid_name, "at", location
				);
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
