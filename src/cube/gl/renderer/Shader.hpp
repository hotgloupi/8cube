#ifndef  CUBE_GL_RENDERER_SHADER_HPP
# define CUBE_GL_RENDERER_SHADER_HPP

# include "fwd.hpp"

namespace cube { namespace gl { namespace renderer {

	class CUBE_API Shader
	{
	public:
		virtual
		~Shader() {}

		friend class ShaderProgram;

	/**************************************************************************
	 * Interface to implement.
	 */
	};

}}}

#endif
