#include "ShaderProgram.hpp"

#include "Exception.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "State.hpp"

#include <etc/log.hpp>
#include <etc/to_string.hpp>

namespace cube { namespace gl { namespace renderer {

	///////////////////////////////////////////////////////////////////////////
	// ShaderProgramParameter

	void ShaderProgramParameter::_set(Texture& texture)
	{
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
			{
				texture_unit = _bound_textures.size();
				_bound_textures[&tex] = texture_unit;
				ETC_LOG.debug(
					"First time binding", tex, "to unit", texture_unit
				);
			}
			else
			{
				texture_unit = it->second;
				ETC_LOG.debug(
					tex, "assigned to unit", texture_unit
				);
			}
		}

		tex.bind_unit(texture_unit, param);
	}

	void ShaderProgram::_unbind() ETC_NOEXCEPT
	{
		ETC_LOG.debug("Clear texture unit map");
		_bound_textures.clear();
	}

	ShaderProgram::ParameterMap&
	ShaderProgram::_parameters()
	{
		if (_parameters_map.get() == nullptr)
		{
			ETC_LOG.debug(this, "Creating the parameters map");
			auto list = _fetch_parameters();
			ParameterMap map;
			for (auto& param : list)
			{
				assert(param.get() != nullptr);
				std::string name = param->name();
				map.emplace(std::move(name), std::move(param));
			}
			_parameters_map.reset(new ParameterMap{std::move(map)});
		}
		return *(_parameters_map.get());
	}

	ShaderProgramParameter&
	ShaderProgram::parameter(std::string const& name)
	{
		ETC_TRACE.debug("Retreive parameter", name);
		auto it = _parameters().find(name);
		if (it != _parameters_map->end())
		{
			ETC_LOG.debug("Found", name, "in cache");
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

}}} // !cube::gl::renderer
