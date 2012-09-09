#ifndef  CUBE_GL_RENDERER_SHADERPROGRAM_HPP
# define CUBE_GL_RENDERER_SHADERPROGRAM_HPP

# include "Drawable.hpp"

# include <etc/types.hpp>

# include <memory>
# include <set>
# include <unordered_map>

namespace cube { namespace gl { namespace renderer {

	class ShaderProgram;

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
		: public Drawable
	{
	private:
		std::set<Shader*> _shaders;
		bool              _finalized;

	public:
		ShaderProgram()
			: _shaders{}
			, _finalized{false}
		{}
		virtual
		~ShaderProgram()
		{ _finalized = true; this->clear(true); }

	/**************************************************************************
	 * Common program behavior.
	 */
	public:
		/**
		 * Attach the shader to the program.
		 */
		void push_shader(std::unique_ptr<Shader>&& shader);

		/**
		 * Finalize the program, must be called before rendering.
		 * When the parameter clear is true, shaders are deleted.
		 */
		void finalize(bool clear = true);

		/**
		 * Delete contained shaders and parameters if parameters_too is true.
		 */
		void clear(bool parameters_too = false);

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
		 * Do graphic operation to attach the shader to the program.
		 * The shader has already been saved.
		 */
		virtual
		void _push_shader(Shader const& shader) = 0;

		/**
		 * Finalize the program (makes it ready for being rendered).
		 */
		virtual
		void _finalize() = 0;

		/**
		 * Fetch a new shader parameter.
		 */
		virtual
		ParameterPtr _fetch_parameter(std::string const& name) = 0;
	};

}}}

#endif
