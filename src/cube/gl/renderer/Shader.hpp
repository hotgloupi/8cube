#ifndef  CUBE_GL_RENDERER_SHADER_HPP
# define CUBE_GL_RENDERER_SHADER_HPP

# include "Drawable.hpp"
# include "Exception.hpp"

# include <string>
# include <vector>

namespace cube { namespace gl { namespace renderer {

	class ShaderProgram;

	class Shader
	{
	private:
		std::vector<std::string>    _sources;
		bool                        _finalized;

	public:
		Shader()
			: _sources{}
			, _finalized{false}
		{}

		virtual
		~Shader() {}

	/**************************************************************************
	 * Common behavior.
	 */
	public:
		inline
		void push_source(std::string const& source)
		{ _sources.push_back(source); }

		inline
		void push_source(std::string&& source)
		{ _sources.push_back(std::move(source)); }
	private:
		//  Called by a shader program, forwards the call to _finalize().
		inline
		void finalize()
		{
			if (_finalized)
				return;
			if (_sources.size() == 0)
				throw Exception{"Cannot finalize an empty shader."};
			this->_finalize(_sources);
			_finalized = true;
		}
		friend class ShaderProgram;

	/**************************************************************************
	 * Interface to implement.
	 */
	protected:
		/**
		 * Prepare the shader to be used by a shader program.
		 */
		virtual
		void _finalize(std::vector<std::string> const& sources) = 0;
	};

}}}

#endif
