#ifndef  CUBE_GL_RENDERER_SHADER_HPP
# define CUBE_GL_RENDERER_SHADER_HPP

# include <memory>

namespace cube { namespace gl { namespace renderer {

	class Shader;
	class ShaderProgram;

	typedef std::unique_ptr<Shader> ShaderPtr;

	class Shader
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
