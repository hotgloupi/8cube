#ifndef  CUBE_GL_OPENGL_SHADERPROGRAM_HPP
# define CUBE_GL_OPENGL_SHADERPROGRAM_HPP

# include <cube/gl/renderer/ShaderProgram.hpp>

# include "_opengl.hpp"

namespace cube { namespace gl { namespace renderer { namespace opengl {

	class ShaderProgram
		: public renderer::ShaderProgram
	{
		ETC_LOG_COMPONENT("cube.gl.renderer.opengl.ShaderProgram");
	public:
		typedef renderer::ShaderProgram Super;
	private:
		GLuint          _id;

	public:
		ShaderProgram(std::vector<ShaderPtr>&& shaders);

		virtual
		~ShaderProgram();

		inline GLuint opengl_id() const ETC_NOEXCEPT { return _id; }

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
		void _unbind() ETC_NOEXCEPT override;

	};

}}}}

#endif
