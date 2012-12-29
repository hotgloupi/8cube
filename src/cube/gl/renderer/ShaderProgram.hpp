#ifndef  CUBE_GL_RENDERER_SHADERPROGRAM_HPP
# define CUBE_GL_RENDERER_SHADERPROGRAM_HPP

# include "Bindable.hpp"

# include <etc/types.hpp>

# include <memory>
# include <unordered_map>
# include <vector>

namespace cube { namespace gl { namespace renderer {

	class ShaderProgram;

	typedef std::unique_ptr<ShaderProgram> ShaderProgramPtr;

	/**
	 * Represent a shader parameter.
	 */
	class ShaderProgramParameter
	{
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
		virtual
		void operator =(matrix_type const& value) = 0;

		virtual
		void operator =(int32_t value) = 0;

		/**
		 * Forward to ShaderProgram::bind_texture_unit.
		 * Should not be overridden.
		 */
		virtual
		void operator =(Texture& texture);
	};

	class ShaderProgram
		: public Bindable
	{
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

		/**
		 * Find an already loaded parameter.
		 */
		ShaderProgramParameter* find_parameter(std::string const& name);

		/**
		 * Update matrix parameters for the given matrix kind.
		 */
		virtual
		void update(MatrixKind kind, matrix_type const& matrix);

		/**
		 * Bind a texture unit to a named shader program parameter.
		 */
		virtual
		void bind_texture_unit(Texture& tex, ShaderProgramParameter& param);

	protected:
		/**
		 * Bindable::_bind(State const&) method is implemented here.
		 * It forward to _bind() and update all parameters from the state.
		 */
		void _bind(State const& state) override;

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
		 * @brief Enable the shader program.
		 */
		virtual
		void _bind() = 0;

		/**
		 * @brief Fetch all shader program parameters.
		 */
		virtual
		std::vector<ParameterPtr> _fetch_parameters() = 0;
	};

}}}

#endif
