#ifndef  CUBE_GL_RENDERER_SHADERPROGRAM_HPP
# define CUBE_GL_RENDERER_SHADERPROGRAM_HPP

# include "Bindable.hpp"

# include <etc/types.hpp>

# include <memory>
# include <unordered_map>

namespace cube { namespace gl { namespace renderer {

	class ShaderProgram;

	typedef std::unique_ptr<ShaderProgram> ShaderProgramPtr;

	/**
	 * Represent a shader parameter.
	 */
	class ShaderProgramParameter
	{
	protected:
		ShaderProgram& _program;

	public:
		ShaderProgramParameter(ShaderProgram& program)
			: _program(program)
		{}

		virtual
		~ShaderProgramParameter() {}

		ShaderProgram& program() { return _program; }

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
		: public Bindable<>
	{
	public:
		virtual
		~ShaderProgram();

	/**************************************************************************
	 * Common program behavior.
	 */
	public:
		friend class ShaderProgramParameter;
		typedef std::unique_ptr<ShaderProgramParameter> ParameterPtr;

	private:
		/// Indexes paramaters with their name.
		std::unordered_map<std::string, ParameterPtr>   _parameters;
		std::unordered_map<Texture*, etc::size_type>    _bound_textures;

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

	/**************************************************************************
	 * Interface to implement.
	 */
	protected:
		/**
		 * Fetch a new shader parameter.
		 */
		virtual
		ParameterPtr _fetch_parameter(std::string const& name) = 0;
	};

}}}

#endif
