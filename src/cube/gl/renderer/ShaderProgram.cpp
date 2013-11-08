#include "ShaderProgram.hpp"

#include "Exception.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "State.hpp"

#include <etc/log.hpp>
#include <etc/to_string.hpp>

namespace cube { namespace gl { namespace renderer {

	ETC_LOG_COMPONENT("cube.gl.renderer.ShaderProgram");

	///////////////////////////////////////////////////////////////////////////
	// ShaderProgramParameter

	void ShaderProgramParameter::operator =(Texture& texture)
	{
		ETC_LOG_SUB_COMPONENT("Parameter");
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

	ShaderProgram::ParameterMap&
	ShaderProgram::_parameters()
	{
		if (_parameters_map.get() == nullptr)
		{
			auto list = _fetch_parameters();
			_parameters_map.reset(new ParameterMap{});
			for (auto& param : list)
			{
				assert(param.get() != nullptr);
				std::string name = param->name();
				_parameters_map->emplace(std::move(name), std::move(param));
			}
		}
		return *(_parameters_map.get());
	}

	ShaderProgramParameter&
	ShaderProgram::parameter(std::string const& name)
	{
		auto it = _parameters().find(name);
		if (it != _parameters_map->end())
		{
			ETC_TRACE.debug("Retreived parameter", name, "in cache !");
			assert(it->second != nullptr);
			return *(it->second);
		}
		throw Exception{"Shader parameter '" + name + "' not found."};
	}

	ShaderProgramParameter*
	ShaderProgram::find_parameter(std::string const& name)
	{
		auto it = _parameters().find(name);
		if (it == _parameters_map->end())
			return nullptr;
		return it->second.get();
	}

	void
	ShaderProgram::update(State const& state)
	{
		struct MatrixGetter
		{
			std::string name;
			matrix_type const& (State::*getter)() const;
		};
		static MatrixGetter matrix_get[] = {
			{"cube_ModelMatrix", &State::model},
			{"cube_ViewMatrix", &State::view},
			{"cube_ProjectionMatrix", &State::projection},
			{"cube_ModelViewProjectionMatrix", &State::mvp},
		};

		Guard guard(*this);
		auto& map = _parameters();
		auto end = map.end();
		for (auto const& matrix: matrix_get)
		{
			auto it = map.find(matrix.name);
			if (it != end)
			{
				*(it->second) = (state.*(matrix.getter))(); // beuark
			}
		}
	}

}}} // !cube::gl::renderer
