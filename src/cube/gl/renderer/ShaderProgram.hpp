#ifndef  CUBE_GL_RENDERER_SHADERPROGRAM_HPP
# define CUBE_GL_RENDERER_SHADERPROGRAM_HPP

# include "Bindable.hpp"

# include <cube/resource/Resource.hpp>

# include <etc/types.hpp>
# include <etc/log/component.hpp>

# include <memory>
# include <unordered_map>
# include <vector>

namespace cube { namespace gl { namespace renderer {

	/**
	 * Represent a shader parameter.
	 */
	class CUBE_API ShaderProgramParameter
	{
		ETC_LOG_COMPONENT("cube.gl.renderer.ShaderProgramParameter");
	protected:
		ShaderProgram&  _program;
		std::string     _name;

	public:
		ShaderProgramParameter(ShaderProgram& program,
		                       std::string const& name)
			: _program(program)
			, _name{name}
		{}

		virtual
		~ShaderProgramParameter() {}

		inline
		ShaderProgram& program() { return _program; }

		inline
		std::string const& name() const { return _name; }

	public:
		template<typename T>
		inline
		void operator =(T&& value)
		{ this->set(std::forward<T>(value)); }

		template<typename T>
		inline
		void set(T&& value)
		{ this->_set(std::forward<T>(value)); }

	protected:
		virtual void _set(matrix_type const& value) = 0;
		virtual void _set(int32_t value) = 0;
		virtual void _set(vector::vec3f const& value) = 0;

		/**
		 * Forward to ShaderProgram::bind_texture_unit.
		 * Should not be overridden.
		 */
		virtual void _set(Texture& texture);
	};

	class CUBE_API ShaderProgram
		: public Bindable
		, public resource::Resource
	{
		ETC_LOG_COMPONENT("cube.gl.renderer.ShaderProgram");
	public:
		virtual
		~ShaderProgram();

	/**************************************************************************
	 * Common program behavior.
	 */
	public:
		friend class ShaderProgramParameter;
		typedef std::unique_ptr<ShaderProgramParameter>         ParameterPtr;
		typedef std::unordered_map<std::string, ParameterPtr>   ParameterMap;
		typedef std::unique_ptr<ParameterMap>                   ParameterMapPtr;
		typedef std::unordered_map<Texture*, etc::size_type>    TextureMap;

	private:
		ParameterMapPtr _parameters_map;
		TextureMap      _bound_textures;

	public:
		/**
		 * Retreive a shader parameter by its name.
		 */
		ShaderProgramParameter& parameter(std::string const& name);
		inline
		ShaderProgramParameter& operator [](std::string const& name)
		{ return this->parameter(name); }

		/**
		 * Find an already loaded parameter.
		 */
		ShaderProgramParameter* find_parameter(std::string const& name);

		/**
		 * Bind a texture unit to a named shader program parameter.
		 */
		virtual
		void bind_texture_unit(Texture& tex, ShaderProgramParameter& param);

	protected:
		/**
		 * @brief Retreive all parameters.
		 */
		ParameterMap& _parameters();

		friend struct InternalGuard<ShaderProgram>;
		typedef InternalGuard<ShaderProgram> Guard;

	/**************************************************************************
	 * Interface to implement.
	 */
	protected:
		/**
		 * @brief Fetch all shader program parameters.
		 */
		virtual
		std::vector<ParameterPtr> _fetch_parameters() = 0;
	};

}}}

#endif
