#include "ShaderProgram.hpp"
#include "Shader.hpp"

#include <etc/to_string.hpp>

namespace cube { namespace gl { namespace renderer {

	void ShaderProgram::push_shader(std::unique_ptr<Shader>&& shader)
	{
		if (_finalized)
			throw Exception{"Cannot modify a finalized shader"};
		if (!shader)
			throw Exception{"Trying to insert a null shader"};
		shader->finalize();
		// just ptr.get() in case of insert() throws.
		_shaders.insert(shader.get());
		// insertion done, next call releases the pointer first.
		_push_shader(*shader.release());
	}

	void ShaderProgram::finalize(bool clear /* = true */)
	{
		if (_shaders.size() == 0)
			throw Exception{"Cannot finalize an empty shader program."};
		if (_finalized)
			throw Exception{"Already finalized shader program."};
		_finalize();
		_finalized = true;
		if (clear)
			this->clear();
	}

	void ShaderProgram::clear(bool parameters_too)
	{
		if (!_finalized)
			throw Exception{"ShaderProgram not yet finalized."};
		for (Shader* shader: _shaders)
			delete shader;
		_shaders.clear();

		if (parameters_too)
			_parameters.clear();
	}

	ShaderProgram::Parameter&
	ShaderProgram::parameter(std::string const& name)
	{
		if (!_finalized)
			throw Exception{"Shader is not finalized"};
		auto it = _parameters.find(name);
		if (it != _parameters.end())
		{
			assert(it->second != nullptr);
			return *(it->second);
		}

		auto param = this->_fetch_parameter(name);

		if (!param)
			throw Exception{"Shader parameter '" + name + "' not found."};

		auto res = _parameters.insert(std::make_pair(name, std::move(param)));
		return *(res.first->second);
	}

	ShaderProgram::Parameter*
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
			if (Parameter* parameter = this->find_parameter(name))
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
