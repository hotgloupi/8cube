#ifndef  CUBE_GL_OPENGL_SHADERPROGRAM_HPP
# define CUBE_GL_OPENGL_SHADERPROGRAM_HPP

# include "../Shader.hpp"
# include "../ShaderProgram.hpp"

# include "_opengl.hpp"

namespace cube { namespace gl { namespace renderer { namespace opengl {

	class Shader;

	class ShaderProgram
		: public renderer::ShaderProgram
	{
	private:
		GLuint  _id;
		bool    _finalized;

	public:
		ShaderProgram();

		virtual
		~ShaderProgram();

	/**************************************************************************
	 * renderer::Shader interface.
	 */
	protected:
		virtual
		void _push_shader(renderer::Shader const& shader);
		virtual
		void _finalize();
		virtual
		ParameterPtr _fetch_parameter(std::string const& name);

	/**************************************************************************
	 * renderer::Drawable interface.
	 */
	protected:
		virtual
		void _bind();
		virtual
		void _unbind();

	};

}}}}

#endif
