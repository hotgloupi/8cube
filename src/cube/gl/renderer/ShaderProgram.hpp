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
		{ this->clear(true); }

	/**************************************************************************
	 * Common program behavior.
	 */
	public:
		/**
		 * Attach the shader to the program.
		 */
		inline
		void push_shader(std::unique_ptr<Shader>&& shader)
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

		/**
		 * Finalize the program, must be called before rendering.
		 * When the parameter clear is true, shaders are deleted.
		 */
		inline
		void finalize(bool clear = true)
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
			typedef Renderer::matrix_type matrix_type;

		public:
			virtual
			~Parameter() {}

			virtual
			Parameter& operator =(matrix_type const& value) = 0;
		};

		typedef std::unique_ptr<Parameter> ParameterPtr;

	private:
		/// Indexes paramaters with their name.
		std::unordered_map<std::string, ParameterPtr> _parameters;

	public:
		/**
		 * Retreive a shader parameter by its name.
		 */
		Parameter& parameter(std::string const& name)
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

			auto res = _parameters.insert({name, std::move(param)});
			return *(res.first->second);
		}

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
