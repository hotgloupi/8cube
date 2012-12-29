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
	 * renderer::ShaderProgram interface.
	 */
	protected:
		std::vector<ParameterPtr> _fetch_parameters() override;

		void _bind() override;

	/**************************************************************************
	 * renderer::Bindable interface.
	 */
	protected:
		void _unbind() override;

	};

}}}}

#endif
