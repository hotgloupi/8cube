#ifndef  CUBE_GL_OPENGL_SHADER_HPP
# define CUBE_GL_OPENGL_SHADER_HPP

# include <cube/gl/renderer/constants.hpp>
# include <cube/gl/renderer/Shader.hpp>

# include "_opengl.hpp"

namespace cube { namespace gl { namespace renderer { namespace opengl {

	class Shader
		: public renderer::Shader
	{
	public:
		renderer::ShaderType const type;
	private:
		GLuint _id;

	public:
		Shader(renderer::ShaderType const type);
		virtual ~Shader();

	public:
		GLuint id() const
		{ return _id; }

	protected:
		virtual
		void _finalize(std::vector<std::string> const& sources);
	};

}}}}

#endif
