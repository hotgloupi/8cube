#ifndef  CUBE_GL_OPENGL_SHADERPROGRAM_HPP
# define CUBE_GL_OPENGL_SHADERPROGRAM_HPP

# include "../ShaderProgram.hpp"

# include "_opengl.hpp"

namespace cube { namespace gl { namespace renderer { namespace opengl {

	class ShaderProgram
		: public renderer::ShaderProgram
	{
	public:
		typedef std::unique_ptr<renderer::Shader> ShaderPtr;

	private:
		GLuint          _id;

	public:
		ShaderProgram(std::vector<ShaderPtr>&& shaders);

		virtual
		~ShaderProgram();

	/**************************************************************************
	 * renderer::Shader interface.
	 */
	protected:
		virtual
		ParameterPtr _fetch_parameter(std::string const& name);

	/**************************************************************************
	 * renderer::Bindable interface.
	 */
	protected:
		virtual
		void _bind();
		virtual
		void _unbind();

	};

}}}}

#endif
