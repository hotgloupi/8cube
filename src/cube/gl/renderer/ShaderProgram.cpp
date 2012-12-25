#include "ShaderProgram.hpp"

#include "Exception.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

#include <etc/log.hpp>
#include <etc/to_string.hpp>

namespace cube { namespace gl { namespace renderer {

	ETC_LOG_COMPONENT("cube.gl.renderer.ShaderProgram");

	///////////////////////////////////////////////////////////////////////////
	// ShaderProgramParameter

	void ShaderProgramParameter::operator =(Texture& texture)
	{
		ETC_LOG_COMPONENT("cube.gl.renderer.ShaderProgramParameter");
		ETC_TRACE.debug("Bind a texture to a shader program parameter");
		_program.bind_texture_unit(texture, *this);
	}

	///////////////////////////////////////////////////////////////////////////
	// ShaderProgram

	ShaderProgram::~ShaderProgram()
	{}

	void ShaderProgram::bind_texture_unit(Texture& tex,
	                                      ShaderProgramParameter& param)
	{
		etc::size_type texture_unit;

		{
			auto it = _bound_textures.find(&tex);
			if (it == _bound_textures.end())
				texture_unit = _bound_textures[&tex] = _bound_textures.size();
			else
				texture_unit = _bound_textures[&tex];
		}

		tex.bind_unit(texture_unit, param);

	}

	ShaderProgramParameter&
	ShaderProgram::parameter(std::string const& name)
	{
		auto it = _parameters.find(name);
		if (it != _parameters.end())
		{
			ETC_TRACE.debug("Retreived parameter", name, "in cache !");
			assert(it->second != nullptr);
			return *(it->second);
		}

		bool temp_bind = !this->_bound();
		if (temp_bind)
			this->_bind();
		ETC_TRACE.debug("Fetching parameter", name, "from the program");
		auto param = this->_fetch_parameter(name);
		if (temp_bind)
			this->_unbind();

		if (!param)
			throw Exception{"Shader parameter '" + name + "' not found."};

		auto res = _parameters.insert(std::make_pair(name, std::move(param)));
		return *(res.first->second);
	}

	ShaderProgramParameter*
	ShaderProgram::find_parameter(std::string const& name)
	{
		auto it = _parameters.find(name);
		if (it == _parameters.end())
			return nullptr;
		return it->second.get();
	}

	void
	ShaderProgram::update(MatrixKind kind,
	                      matrix_type const& matrix)
	{
		ETC_TRACE.debug("Update ShaderProgram matrix", kind);
		static std::string const map[] = {
			"cube_ModelMatrix",
			"cube_ViewMatrix",
			"cube_ProjectionMatrix",
			"cube_ModelViewProjectionMatrix",
		};
		static_assert((int)MatrixKind::model == 0, "Wrong index");
		static_assert((int)MatrixKind::view == 1, "Wrong index");
		static_assert((int)MatrixKind::projection == 2, "Wrong index");
		static_assert((int)MatrixKind::mvp == 3, "Wrong index");
		std::string name;
		switch (kind)
		{
		case MatrixKind::model:
		case MatrixKind::view:
		case MatrixKind::projection:
		case MatrixKind::mvp:
			name = map[(unsigned int) kind];
			assert(name.size() > 0 && "Name not registered ?");
			if (auto parameter = this->find_parameter(name))
				*parameter = matrix;
			break;
		default:
			throw Exception{
				"Unknown MatrixKind '" +
				etc::to_string((int) kind)
				+ "'."
			};
		}
	}

}}} // !cube::gl::renderer
