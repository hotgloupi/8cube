#ifndef  CUBE_GL_RENDERER_SHADERPROGRAM_HPP
# define CUBE_GL_RENDERER_SHADERPROGRAM_HPP

# include <cube/gl/renderer.hpp>

# include "Drawable.hpp"
# include "Exception.hpp"
# include "Shader.hpp"

# include <memory>
# include <set>
# include <unordered_map>

namespace cube { namespace gl { namespace renderer {

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
		/**
		 * Represent a shader parameter.
		 */
		class Parameter
		{
		public:
			virtual
			~Parameter() {}

			virtual
			Parameter& operator =(matrix_type const& value) = 0;
		};
		friend class Parameter;

		typedef std::unique_ptr<Parameter> ParameterPtr;

	private:
		/// Indexes paramaters with their name.
		std::unordered_map<std::string, ParameterPtr> _parameters;

	public:
		/**
		 * Retreive a shader parameter by its name.
		 */
		Parameter& parameter(std::string const& name);

		Parameter* find_parameter(std::string const& name);

		virtual
		void update(MatrixKind kind, matrix_type const& matrix);

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
